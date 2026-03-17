
import express from "express";
import { Worker } from "worker_threads";

const app = express();
app.use( express.json() );

const worker = new Worker( "./worker.js" );
export const pendingRequests = new Map(); // Track callbacks for async worker responses

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

const PORT = 3000;
app.listen(
	PORT,
	"0.0.0.0",
	() => {
		console.log( `Phonomask server running at ${PORT}` );
	}
);


app.get(
	"/",
	( req, res ) =>
	{
		try
		{
			res.json(
				{
					message: "Welcome to Phonomask, a bitmap-based phonology engine"
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
		const { table_str } = req.body;

		console.log( "table_str: ", table_str );

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
						payload: { table_str }
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
			return res.status( 468 ).send( response.error );
		}
		res.json(
			{
				sessionId,
				result: response.result
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
					payload: { rule, word } 
				}
			);
		}
	);

	const response = await resultProm;

	console.log( "worker apply_rule response: ", response );

	if ( response.type === "ERROR" ) 
	{
		return res.status( 500 ).send( response.error );
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
					payload: { rules, word } 
				}
			);
		}
	);

	const response = await resultProm;

	console.log( "worker apply_many response: ", response );

	if ( response.type === "ERROR" ) 
	{
		return res.status( 500 ).send( response.error );
	}
	res.json( { result: response.result } );
}

app.post( "/api/create_session", create_session );

app.post( "/api/apply_rule", apply_rule );

app.post( "/api/apply_many", apply_many );

