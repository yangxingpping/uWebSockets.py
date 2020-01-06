import uwebsocketspy as uWS

def handler(res, req):
	res.end("Hello Python!")

app = uWS.App({
	"some": "option"
}).listen(handler)
