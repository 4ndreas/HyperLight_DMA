#ifndef BUFFERED_ARTNET_H
#define BUFFERED_ARTNET_H

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <cassert>

#define ARTNET_PAKET_MIN_SIZE 20

static const uint8_t artnet_header_fixed[] = { 'A', 'r', 't', '-', 'N', 'e', 't', 0, 0x00, 0x50, 0, 14 };

template<unsigned universes>
class BufferedArtnet
{
	uint8_t data[universes][512];
	EthernetUDP udp;


public:
	BufferedArtnet(): data{0} {
	}

	void begin() {
		udp.begin(6454);
	}

	void end() {
		udp.stop();
	}

	uint8_t* getUniverseData(unsigned u) {
		assert(u < universes);
		return data[u];
	}

	bool read() {
		unsigned packet_size = udp.parsePacket();

		if(packet_size == 0)
			return false;

		if(packet_size < ARTNET_PAKET_MIN_SIZE)
			return true;

		uint8_t header[18];
		udp.read(header, 18);

		for(int i=0; i < 12; ++i)
			if(header[i] != artnet_header_fixed[i])
				return true;

		uint16_t u = header[14] | (uint16_t(header[15]) << 8);
		uint16_t size = header[17] | (uint16_t(header[16]) << 8);

		if(u >= universes)
			return true;

		if(size > 512)
			return true;

		int read = udp.read(data[u], size);
		assert(read == size);

		return true;
	}
};

#endif
