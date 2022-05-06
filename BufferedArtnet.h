#ifndef BUFFERED_ARTNET_H
#define BUFFERED_ARTNET_H

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <cassert>

#define ARTNET_PAKET_MIN_SIZE 20

static const uint8_t artnet_header_fixed[] = { 'A', 'r', 't', '-', 'N', 'e', 't', 0};

template<unsigned universes>
class BufferedArtnet
{
	uint8_t data[universes][512];
	uint8_t new_data[universes];
	uint8_t last_seq[universes];
	unsigned lost_frames[universes];
	EthernetUDP udp;
	bool sync_pending;
	unsigned sync_received_time;
	unsigned read_max;


public:

	IPAddress remote;

	BufferedArtnet(): data{0}, last_seq{0}, lost_frames{0}, sync_pending(false), sync_received_time(0) {
	}

	void begin() {
		udp.begin(6454);
	}

	void end() {
		udp.stop();
	}

	uint8_t newUniverseData(unsigned u) {
		assert(u < universes);
		return new_data[u];
	}


	uint8_t* getUniverseData(unsigned u) {
		assert(u < universes);
		new_data[u] = 0;
		return data[u];
	}

	bool syncMode() {
		return (millis() - sync_received_time) < 4000;
	}

	bool syncPending() {
		return sync_pending;
	}

	void clearSyncPending() {
		sync_pending = false;
	}

	bool read() {
		unsigned packet_size = udp.parsePacket();

		remote = udp.remoteIP();	// note Host IP

		if(packet_size == 0)
			return false;

		uint8_t header[12];
		if(udp.read(header, 12) != 12)
			return true;

		for(int i=0; i < 8; ++i)
			if(header[i] != artnet_header_fixed[i])
				return true;

		uint16_t op = header[8] | (uint16_t(header[9]) << 8);
		uint16_t ver = header[11] | (uint16_t(header[10]) << 8);

		if(ver < 14)
			return true;

		if(op == 0x5200) { // ArtSync
			sync_pending = true;
			sync_received_time = millis();
			return true;
		}

		if(op != 0x5000) // ArtDMX
			return true;

		uint8_t art_dmx_header[6];
		if(udp.read(art_dmx_header, 6) != 6)
			return true;

		uint8_t seq = art_dmx_header[0];
		uint16_t u = art_dmx_header[2] | (uint16_t(art_dmx_header[3]) << 8);
		uint16_t size = art_dmx_header[5] | (uint16_t(art_dmx_header[4]) << 8);

		if(u >= universes)
			return true;

		if(size > 512)
			return true;

		if(seq != 0) {
			if(last_seq[u] != 0) {
				uint8_t lost = seq - last_seq[u] - 1;
				if(seq <= last_seq[u]) lost--; // wrap around, seq skips 0
				lost_frames[u] += lost;
			}

			last_seq[u] = seq;
		}

		new_data[u] = 1;

		unsigned read_start = micros();
		int read = udp.read(data[u], size);
		unsigned read_time = micros() - read_start;
		read_max = read_time > read_max ? read_time : read_max;
		assert(read == size);

		return true;
	}
};

#endif
