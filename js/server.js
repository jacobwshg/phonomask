
import express from "express";
import { Worker } from "worker_threads";

const app = express();
app.use( express.json() );

const worker = new Worker( "./worker.js" );
export const pendingRequests = new Map(); // Track callbacks for async worker responses

const PORT = process.env.PORT || 3000;
app.listen(
	PORT,
	"0.0.0.0",
	() => {
		console.log( `Phonomask server running at ${PORT}` );
	}
);

worker.on(
	"message",
	( msg ) =>
	{
		if ( msg.type === "READY" ) 
		{
			console.log( "Worker ready" );
		}

		// Find the original HTTP request and resolve it
		if ( msg.sessionId )
		{
			console.log( `worker msg has sId ${msg.sessionId}` );
			const resolver = pendingRequests.get( msg.sessionId );
			if ( resolver )
			{
				resolver( msg );
				pendingRequests.delete( msg.sessionId );
			}
		}
});

app.get(
	"/",
	( req, res ) =>
	{
		try
		{
			res.json(
				{
					message: "Welcome to Phonomask, a bitmask-based phonology engine"
				}
			);
		}
		catch( e )
		{
			console.error( e );
			res.status( 500 ).json(
				{
					message: e.message
				}
			);
		}
	}
);

async function
create_session( req, res )
{
	//try
	//{
		const { } = req.body;

		const { v4 } = await import('uuid');
		let sessionId = v4();

		console.log( `server created sessionId ${sessionId}` );

		const resultProm = new Promise(
			( resolve ) =>
			{
				pendingRequests.set( sessionId, resolve );

				worker.postMessage(
					{
						type: "CREATE_SESSION",
						sessionId,
						worker_payload: { }
					}
				);
			}
		);

		console.log( "server waiting for worker create_session promise" );
		const response = await resultProm;
		console.log( "worker create_session promise resolved" );
		if ( response.type === "ERROR" )
		{
			sessionId = -1;
			console.error( "server create_session response error" );
			res.status( 500 ).send( response.error );
			return;
		}

		const table_str = response.result;

		let result = "Welcome to Phonomask, a bitmask-based phonology engine.\n\nSample feature profile loaded:\n\n";
		result += table_str;
		result += "\nYou can download it or upload your own.\n"

		res.json(
			{
				sessionId: sessionId,
				result: result
			}
		);
	//}
	//catch ( e )
	//{
		//res.status( 500 ).send( e.message );
	//}
	console.log( `end of server create_session` );
}

async function
apply_rule( req, res )
{
	const { sessionId, rule, word } = req.body;
	console.log( `server received rule ${rule}, word ${word}` );
	// Create a promise that resolves when the worker replies
	const resultProm = new Promise(
		( resolve ) =>
		{
			pendingRequests.set( sessionId, resolve );
			worker.postMessage(
				{
					type: "APPLY_RULE", 
					sessionId,
					worker_payload: { rule, word } 
				}
			);
		}
	);
	const response = await resultProm;
	console.log( "worker apply_rule response: ", response );
	if ( response.type === "ERROR" ) 
	{
		res.status( 500 ).send( response.error );
	}
	res.json( { result: response.result } );
}

async function
apply_many( req, res )
{
	const { sessionId, rules, word } = req.body;
	const resultProm = new Promise(
		( resolve ) =>
		{
			pendingRequests.set( sessionId, resolve );
			worker.postMessage(
				{
					type: "APPLY_MANY", 
					sessionId,
					worker_payload: { rules, word } 
				}
			);
		}
	);
	const response = await resultProm;

	console.log( "worker apply_many response: ", response );

	if ( response.type === "ERROR" ) 
	{
		res.status( 500 ).send( response.error );
	}
	res.json( { result: response.result } );
}

async function
features_str( req, res )
{
	const { sessionId, segment } = req.body;
	const resultProm = new Promise(
		( resolve ) =>
		{
			pendingRequests.set( sessionId, resolve );
			worker.postMessage(
				{
					type: "FEATURES_STR",
					sessionId,
					worker_payload: { segment }
				}
			);
		}
	);
	const response = await resultProm;
	console.log( "worker features_str response: ", response  );
	if ( response.type === "ERROR" )
	{
		res.status( 500 ).send( response.error );
	}
	res.json( { result: response.result } );
}

async function
delete_session( req, res )
{
	const { sessionId } = req.body;
	const resultProm = new Promise(
		( resolve ) =>
		{
			pendingRequests.set( sessionId, resolve );
			worker.postMessage(
				{
					type: "DELETE_SESSION",
					sessionId,
					worker_payload: { sessionId }
				}
			);
		}
	);
	const response = await resultProm;
	console.log( "worker delete_session response: ", response );
	if ( response.type === "ERROR" )
	{
		res.status( 500 ).send( response.error );
	}
	res.json( { result: response.result } );
}


app.post( "/session", create_session );

app.post( "/api/apply_rule", apply_rule );

app.post( "/api/apply_many", apply_many );

app.get( "/api/features_str", features_str );

app.post( "/api/delete_session", delete_session );

