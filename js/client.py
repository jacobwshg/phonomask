import requests
import json
import uuid

baseurl = "http://localhost:3000"
#baseurl = "https://ec2-3-21-142-111.us-east-2.compute.amazonaws.com"

# Step 1: Create a session
def create_session(  ):
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

def update_profile( sid, table_str ):
	url = baseurl + "/profile"
	body = { "sessionId": sid, "table_str": table_str }
	response = requests.post( url, json=body )
	print( "update_profile response:" )

	print( response )

	json = response.json()
	print( json[ "result" ] )

def get_base_profile( sid ):
	url = baseurl + "/base_profile"
	body = { "sessionId": sid }
	response = requests.get( url, json=body )
	print( "get_base_profile response: " )
	print( response )
	return response.json()[ "result" ]
	
def get_features_str( sid, segment ):
	url = baseurl + "/features_str"
	body = { "sessionId": sid, "segment": segment }
	response = requests.get( url, json=body )
	print( "get_features_str response: " )
	print( response )
	feats_str = response.json()[ "result" ]
	return feats_str

def apply_rule( sid, rule, word ):
	url = baseurl + "/apply_rule"
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
	
def apply_many_rules(session_id, rules, word):
	url = baseurl + "/apply_many"
	body = {
		"sessionId": session_id,
		"rules": rules,  # Python list of strings
		"word": word
	}
	
	response = requests.post( url, json=body )
	print( "\n\napply_many_rules response:" )
	print( response )
	return response.json()

def delete_session( sid ):
	url = baseurl + "/session"
	body = { "sessionId":sid }
	response = requests.delete( url, json=body )
	print( "delete_session response: " )
	print( response )

# Example usage
if __name__ == "__main__":
	# User feature table
	feature_table = """
IPA,cons,syl,voi,lab,bk
p,+,-,-,+,-
t,+,-,-,-,-
k,+,-,-,-,+
d,+,-,+,-,-
g,+,-,+,-,+
a,-,+,+,-,-
u,-,+,+,+,+
"""

	# Create session
	session_id = create_session( )
	print(f"Created session: {session_id}")
	print()

	base_table_str = get_base_profile( session_id )
	bp_path =  "./base_profile.csv"
	with open ( bp_path, "w" ) as bpf:
		bpf.write( base_table_str )
	print( "base profile written to " + bp_path )
	print()

	test_segments = [ "p", "g", "a" ]

	feats_strs = [ get_features_str( session_id, ts ) for ts in test_segments ]
	print( "Feature str test on base profile: " )
	for ts, fstr in zip( test_segments, feats_strs ):
		print( f"[{ ts }] = { fstr }" )
	print()

	word1 = "atparg"
	rules1 = [
		"∅ -> a / [+cons] _ [+cons] ",
		"[+cons, -syl, -son, -voi] -> [+voi] / [-cons, +syl, +son, +voi] _ [-cons, +syl, +son, +voi]",
		"[+voi] -> ∅ / _ # "
	]
	print( "word 1: ")
	print( word1 )
	print( "rule set 1: " )
	for r in rules1:
		print( r )
	print()
	print( "Applying one by one on base feature profile" )
	word = word1
	for r in rules1:
		word = apply_rule( session_id, r, word )
		print( "word after application: " + word )
		print()

	update_profile( session_id, feature_table )
	print()

	feats_strs = [ get_features_str( session_id, ts ) for s in test_segments ]
	print( "Feature str test on base profile: " )
	for ts, fstr in zip( test_segments, feats_strs ):
		print( f"[{ ts }] = { fstr }" )
	print()


	# Apply rules
	word2 = "ptk"
	rules2 = [
		"∅ -> a / [ -syl ] _ [ -syl ] ",
		"a -> u / _ [ +bk ] ",
		"[ -voi ] -> [ +voi ] / [ +voi ]  _ [ +voi ] ",
	]

	print( "word 2: ")
	print( word2 )
	print( "rule set 2: " )
	for r in rules2:
		print( r )
	print( "Applying in batch on user feature profile" )
	result = apply_many_rules( session_id, rules2, word2 )
	print( result )
	print()	

	result = delete_session( session_id )
	print( "Session shutting down." )
	print()

