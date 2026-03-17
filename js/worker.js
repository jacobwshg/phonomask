
import { parentPort } from "worker_threads";
import createPhmaskModule from "./phmask.js";

let phmask = null;
const sessions = new Map();

// Initialize the Wasm module within the worker
createPhmaskModule().then(
	( module ) =>
	{
   		phmask = module;
		console.log( "worker initialized wasm module" );
		parentPort.postMessage(
			{ type: "READY" }
		);
	}
);

/* listen for msg from parent */
parentPort.on(
	"message", 
	( msg ) => 
	{
		const { type, sessionId, payload } = msg;

		try
		{
			console.log( `worker received msg type ${type}` );

			if ( type === "CREATE_SESSION" )
			{
				const sess = new phmask.PhmaskSession();
				console.log( "worker initialized wasm session" );
				const { table_str } = payload;

				sess.populate( table_str ); //stuck

				console.log( "worker wasm session populated" );
				sessions.set( sessionId, sess );
				parentPort.postMessage(
					{
						type: "SUCCESS",
						sessionId: sessionId
					}
				);
			}
			else if ( type === "APPLY_RULE" )
			{
				const sess = sessions.get( sessionId );
				const { rule, word } = payload;
				console.log( `worker received rule ${rule}, word ${word}` );
				const result = sess.apply_rule( rule, word );
				console.log( `worker apply_rule result: ${result}` );
				parentPort.postMessage(
					{
						type: "RESULT",
						sessionId: sessionId,
						result: result
					}
				);
			} 
			else if ( type === "APPLY_MANY" )
			{
				const sess = sessions.get( sessionId );
				let { rules, word } = payload;
				console.log( `worker received rules` );
				for ( let r of rules )
				{
					console.log( "\t", r );
				}
				console.log( `, word ${ word }` );

let results = [];

// Create a StringVec object (not a regular JavaScript array)
for ( let r of rules )
{
	const result = sess.apply_rule( r, word );
	word = result;
	results.push( result );
}
	
				console.log( `worker apply_many result: ${ results }` );
				parentPort.postMessage(
					{
						type: "RESULT",
						sessionId,
						result: results
					}
				);
			} 
			else if ( type === "DELETE_SESSION" )
			{
				const sess = sess.get( sessionId );
				if ( sess )
				{
					sess.delete();
					sessions.delete( sessionId );
				}
			}
		}
		catch ( err )
		{
			parentPort.postMessage(
				{
					type: "ERROR",
					sessionId,
					error: err.message
				}
			);
		}
	}
);

