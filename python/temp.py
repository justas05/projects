import httplib, urllib, base64

headers = {
	    # Request headers
	        'Content-Type': 'multipart/form-data',
		    'Ocp-Apim-Subscription-Key': '{subscription key}',
}

params = urllib.urlencode({
})

try:
    conn = httplib.HTTPSConnection('api.cognitive.microsoft.com')
        conn.request("POST", "/bing/v5.0/images/search?%s" % params, "{body}", headers)
	    response = conn.getresponse()
	        data = response.read()
		    print(data)
		        conn.close()
			except Exception as e:
			    print("[Errno {0}] {1}".format(e.errno, e.strerror))

####################################

########### Python 3.2 #############
import http.client, urllib.request, urllib.parse, urllib.error, base64

headers = {
	    # Request headers
	        'Content-Type': 'multipart/form-data',
		    'Ocp-Apim-Subscription-Key': '{subscription key}',
}

params = urllib.parse.urlencode({
})

try:
    conn = http.client.HTTPSConnection('api.cognitive.microsoft.com')
        conn.request("POST", "/bing/v5.0/images/search?%s" % params, "{body}", headers)
	    response = conn.getresponse()
	        data = response.read()
		    print(data)
		        conn.close()
			except Exception as e:
			    print("[Errno {0}] {1}".format(e.errno, e.strerror))

