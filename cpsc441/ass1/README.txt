This program was tested on Firefox Version 58.0 on 64-bit Linux (Fedora distro) on school network (air-ucsecure). The webpages used to test this proxy were the simple ones provided on the Assignment page:

http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test0.txt
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test1.html
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test2.html
http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test3.html

-------------------------------------
| Configuring the browser (Firefox) |
-------------------------------------

Before compiling, the browser must be configured to the following settings:

1. Under General > Network Proxy > Settings, select Manual proxy configuration with the following settings and leave the rest as default:
	HTTP Proxy: 127.0.0.1	Port: 6969

2. Under Privacy & Security > Cached Web Content, clear the cache by selecting the "Clear Now" button and check "Override automatic cache management" and set the value to 0MB of space. 
This will prevent the cache from overriding the delayed load when the web server responds with a 	304 Not Modified and loads the page immediately.

-------------------------------------
| Compiling and Using the Web Proxy |
-------------------------------------

1. Compile the program by opening a terminal in the folder location where "sloxy.c" is found, then type "gcc sloxy.c -o sloxy" and press enter.

2. To run the proxy, type "./sloxy" and press enter.

3. In Firefox, load any of simple test pages as shown above. test0.txt and test1.html should have no or extremely short delay, test2.html will have a small delay, and test3.html will have a fairly long delay.
