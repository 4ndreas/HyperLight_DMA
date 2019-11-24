/*
 * website.h
 *
 *  Created on: 23.11.2019
 *      Author: ahoel
 */

#ifndef WEBSITE_H_
#define WEBSITE_H_


// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>Arduino Ajax LED Button Control</title>
        <script>
        strLED1 = "";
        strLED2 = "";
        var LED2_state = 0;
        function GetArduinoIO()
        {
            nocache = "&nocache=" + Math.random() * 1000000;
            var request = new XMLHttpRequest();
            request.onreadystatechange = function()
            {
                if (this.readyState == 4) {
                    if (this.status == 200) {
                        if (this.responseXML != null) {
                            // XML file received - contains analog values, switch values and LED states
                            var count;
                            // LED 1
                            if (this.responseXML.getElementsByTagName('LED')[0].childNodes[0].nodeValue === "checked") {
                                document.LED_form.LED1.checked = true;
                            }
                            else {
                                document.LED_form.LED1.checked = false;
                            }

                            // strips
							document.getElementById(this.responseText).innerHTML = this.responseText
                        }
                    }
                }
            }
            // send HTTP GET request with LEDs to switch on/off if any
            request.open("GET", "ajax_inputs" + strLED1 + strLED2 + nocache, true);
            request.send(null);
            setTimeout('GetArduinoIO()', 1000);
            strLED1 = "";
            strLED2 = "";
        }
        // service LEDs when checkbox checked/unchecked
        function GetCheck()
        {
            if (LED_form.LED1.checked) {
                strLED1 = "&LED1=1";
            }
            else {
                strLED1 = "&LED1=0";
            }
        }
        function GetButton1()
        {
            if (LED2_state === 1) {
                LED2_state = 0;
                strLED2 = "&LED2=0";
            }
            else {
                LED2_state = 1;
                strLED2 = "&LED2=1";
            }
        }
    </script>
    <style>
        .IO_box {
            float: left;
            margin: 0 20px 20px 0;
            border: 1px solid blue;
            padding: 0 5px 0 5px;
            width: 120px;
        }
        h1 {
            font-size: 120%;
            color: blue;
            margin: 0 0 10px 0;
        }
        h2 {
            font-size: 85%;
            color: #5734E6;
            margin: 5px 0 5px 0;
        }
        p, form, button {
            font-size: 80%;
            color: #252525;
        }
        .small_text {
            font-size: 70%;
            color: #737373;
        }
    </style>
    </head>
    <body onload="GetArduinoIO()">
        <h1>HyperLight Config</h1>

        <div>
		<tr>
			<td>Universe 0: </td><span id="U0_stat">0</span><td> - </td><td>lastUpdate ms ago</td>
			<td>Universe 1: </td><span id="U1_stat">0</span><td> - </td><td>lastUpdate ms ago</td>
			<td>Universe 2: </td><span id="U2_stat">0</span><td> - </td><td>lastUpdate ms ago</td>
			<td>Universe 3: </td><span id="U3_stat">0</span><td> - </td><td>lastUpdate ms ago</td>
			<td>Universe 4: </td><span id="U4_stat">0</span><td> - </td><td>lastUpdate ms ago</td>
			<td>Universe 5: </td><span id="U5_stat">0</span><td> - </td><td>lastUpdate ms ago</td>
		</tr>
        </div>
	
    </body>
</html>


)rawliteral";


//                            if (this.responseXML.getElementsByTagName('strip')[1].childNodes[0].nodeValue === "0") {
//                                document.getElementById("Universe1_stat").innerHTML = "OK";
//								document.getElementById("switch_txt").innerHTML = this.responseText;
//                            }
//                            else {
//                                document.getElementById("Universe1_stat").innerHTML = "error";
//                            }

//<div class="IO_box">
//    <h2>LED Using Checkbox</h2>
//    <form id="check_LEDs" name="LED_form">
//        <input type="checkbox" name="LED1" value="0" onclick="GetCheck()" />LED 1 (D6)<br /><br />
//    </form>
//</div>
//<div class="IO_box">
//    <h2>LED Using Button</h2>
//    <button type="button" id="LED2" onclick="GetButton1()">LED 2 is OFF (D7)</button><br /><br />
//</div>


// send a standard http response header
//		  client.println("HTTP/1.1 200 OK");
//		  client.println("Content-Type: text/html");
//		  client.println("Connection: close");  // the connection will be closed after completion of the response
//		  client.println("Refresh: 10");  // refresh the page automatically every 5 sec
//		  client.println();
//		  client.println("<!DOCTYPE HTML>");
//		  client.println("<html>");
//
//		  // output the value of each analog input pin
//		  client.print("<h1>HyperLight Info </h1>");
//
//		  client.print("My IP: ");
//		  client.print(ip[0], DEC);client.print(".");
//		  client.print(ip[1], DEC);client.print(".");
//		  client.print(ip[2], DEC);client.print(".");
//		  client.print(ip[3], DEC);
//
//		  client.println("<br />");
//		  client.print("Host: ");
//		  client.print(remoteArd[0]);client.print(".");
//		  client.print(remoteArd[1]);client.print(".");
//		  client.print(remoteArd[2]);client.print(".");
//		  client.print(remoteArd[3]);
//		  client.println("<br />");
//		  client.println("<br />");
//		  client.println("<table border=\"1\">");
//
//		  for (int strip = 0; strip < 16; strip++)
//		  {
//			client.println("<tr>");
//			int lastUpdate = currentTime - leds.getUpdateTime(strip);
//			client.println("<td>");
//			client.print("Universe ");
//			client.print(strip);
//			client.println("</td>");
//			client.println("<td ALIGN=\"CENTER\">");
//			if (lastUpdate < 1000)
//			{
//				client.print("<font color=\"green\"> OK </font>");
//			}
//			else
//			{
//				client.print("<font color=\"red\"> error </font>");
//			}
//			client.println("</td>");
//			client.println("<td>");
//
//			client.print("updated ");
//			client.print(lastUpdate);
//			client.print(" ms ago ");
//
//			client.println("</li>");
//			client.println("</td>");
//			client.println("</tr>");
//		  }
//
//
//		  client.println("</table>");
//		  client.println("</html>");

/*


			client.println("HTTP/1.1 200 OK");

            if (StrContains(HTTP_req, "ajax_inputs")) {
                // send rest of HTTP header
                client.println("Content-Type: text/xml");
                client.println("Connection: keep-alive");
                client.println();
                SetLEDs();
                // send XML file containing input states
                XML_response(client);
            }
            else {  // web page request
                // send rest of HTTP header
                client.println("Content-Type: text/html");
                client.println("Connection: keep-alive");
                client.println();
                // send web page
                client.print(index_html);
            }

// checks if received HTTP request is switching on/off LEDs
// also saves the state of the LEDs
void SetLEDs(void)
{
    if (StrContains(HTTP_req, "LED1=1")) {


    }
    else if (StrContains(HTTP_req, "LED1=0")) {


    }

    if (StrContains(HTTP_req, "LED2=1")) {

    }
    else if (StrContains(HTTP_req, "LED2=0")) {

    }
}

// send the XML file with analog values, switch status
//  and LED status
void XML_response(EthernetClient client)
{
    client.print("<?xml version = \"1.0\" ?>");
    client.print("<inputs>");

	for (int strip = 0; strip < 16; strip++)
	  {
		int lastUpdate = currentTime - leds.getUpdateTime(strip);

		client.print("<U");
		client.print(strip);
		client.print("_stat>");
		if (lastUpdate < 1000)
		{
			client.print("OK");
		}
		else
		{
			client.print("errorc");
		}
		client.print("</U");
		client.print(strip);
		client.print("_stat>");
	  }

	client.print("</inputs>");
}
*/


#endif /* WEBSITE_H_ */
