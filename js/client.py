import requests
import json
import uuid

baseurl = "http://localhost:3000"
#baseurl = "https://ec2-3-21-142-111.us-east-2.compute.amazonaws.com"

# Step 1: Create a session
def create_session(table_str):
	url = baseurl + "/session"
	payload = { }
	
	response = requests.post( url, json=payload )
	
	#while True:
	#	pass

	print( "create_session response:" )
	print( response )

	json = response.json()
	print( json["result"] )
	return json["sessionId"]

def apply_rule( sid, rule, word ):
	url = baseurl + "/api/apply_rule"
	body = {
		"sessionId":sid,
		"rule":rule,
		"word":word
	}
	res = requests.post( url, json=body )
	print( "apply_rule response: " )
	result = res.json()
	print( result )
	return result[ "result" ]
	
# Step 2: Apply multiple rules
def apply_many_rules(session_id, rules, word):
	url = baseurl + "/api/apply_many"
	body = {
		"sessionId": session_id,
		"rules": rules,  # Python list of strings
		"word": word
	}
	
	response = requests.post( url, json=body )
	print( "\n\napply_many_rules response:" )
	print( response )
	print( response.json() )
	print( "\n\n\n" )

	return response.json()

def delete_session( sid ):
	url = baseurl + "/api/delete_session"
	body = { "sessionId":sid }
	res = requests.post( url, json=body )
	print( res )

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

	result = delete_session( session_id )


