# µWebSockets.py
**WIP extension for µWebSockets**
```python
import uwebsocketspy as uWS

def handler(res, req):
	res.end("Hello Python!")

app = uWS.App({
	"some": "option"
}).listen(handler)
```
It performs significantly better than japronto, despite being a far more complete and mature project.
