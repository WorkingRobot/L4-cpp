import requests, json

from collections import defaultdict

ddict = lambda: defaultdict(ddict)
def setref_multi(data, keys, value):
	if len(keys) > 1:
		setref_multi(data[keys[0]], keys[1:], value)
	else:
		data[keys[0]] = value

a = requests.get("https://clientconfig.rpg.riotgames.com/api/v1/config/public").json()
b = ddict()
for k,v in a.items(): setref_multi(b, k.split('.'), v)

with open("f.json", "w") as f: json.dump(b, f, indent=4, sort_keys=True)