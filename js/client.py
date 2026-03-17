import requests
import json
import uuid

baseurl = "http://localhost:3000"

# Step 1: Create a session
def create_session(table_str):
	url = baseurl + "/api/create_session"
	payload = { "table_str": table_str }
	
	response = requests.post(url, json=payload)
	
	#while True:
	#	pass

	print( "create_session response:" )
	print( response )

	data = response.json()
	return data["sessionId"]

def apply_rule( sid, rule, word ):
	url = baseurl + "/api/apply_rule"
	payload = {
		"sessionId":sid,
		"rule":rule,
		"word":word
	}
	res = requests.post( url, json=payload )
	print( "apply_rule response: " )
	result = res.json()
	print( result )
	return result[ "result" ]
	


# Step 2: Apply multiple rules
def apply_many_rules(session_id, rules, word):
	url = baseurl + "/api/apply_many"
	payload = {
		"sessionId": session_id,
		"rules": rules,  # Python list of strings
		"word": word
	}
	
	response = requests.post(url, json=payload)
	print( "\n\napply_many_rules response:" )
	print( response )
	print( response.json() )
	print( "\n\n\n" )

	return response.json()

# Example usage
if __name__ == "__main__":
	# Your feature table
	feature_table = """
IPA,cons,syl,voi,lab,bk
p,+,-,-,+,-
t,+,-,-,-,-
k,+,-,-,-,+
g,+,-,+,-,+
a,-,+,+,-,-
u,-,+,+,+,+
"""

#ə,-,+,+,-,-
	

	with open ( "../lx301-base.csv", "r" ) as tblf:
		feature_table = tblf.read()

	print( "python feature table: " )
	print( feature_table )

	# Create session
	session_id = create_session( feature_table )
	print(f"Created session: {session_id}")
	
	# Apply rules
	rules = [
		"∅ -> a / [ -syl ] _ [ -syl ] ",
		"a -> u / _ [ +bk ] ",
		"[ -voi ] -> [ +voi ] / [ +voi ]  _ [ +voi ] ",
	]

	testword = "ptk"

	word = testword
	print( "word: " + word )
	for r in rules:
		print( "rule: " + r )
		word = apply_rule( session_id, r, word )
		print( "word after application: " + word )

	word = testword
	result = apply_many_rules( session_id, rules, word )
	print(f"Results: {result['result']}")

