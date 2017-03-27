from hashlib import sha1
import os, getpass

name = raw_input()
login = raw_input()
password = getpass.getpass('Password:')
os.mkdir("../hlavnykomp/uploady/"+login)
s = "  ('"+login+"' , u'"+name+"', b'"+sha1('abc'+password+'def').hexdigest()+"' ),\n"
with open('../hlavnykomp/webconfig') as subor:
	lines = subor.readlines()
	lines = lines[:3] + list(s) + lines[3:]

with open('../hlavnykomp/webconfig', 'w') as subor:
	for line in lines:
		subor.write(line)


with open('../hlavnykomp/srvconfig') as subor:
	lines = subor.readlines()
	lines = lines[:4] + list("  '"+name+"'\n") + lines[4:]
	lines[0] = ' '.join(lines[0].split()[:-1] + [login, ')']) + '\n'

with open('../hlavnykomp/srvconfig', 'w') as subor:
	for line in lines:
		subor.write(line)
