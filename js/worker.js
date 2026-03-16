
import { parentPort } from "worker_threads";
import createPhmaskModule from "./phmask.js";

let phmask = null;
const sessions = new Map();

// Initialize the Wasm module within the worker
createPhmaskModule().then(
	( module ) =>
	{
   		phmask = module;
		console.log( "worker initialzied wasm module" );
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
				sess.populate( payload );
				sessions.set( sessionId, sess );
				parentPort.postMessage(
					{
						type: "SUCCESS",
						sessionId
					}
				);
			}
			else if ( type === "APPLY_RULE" )
			{
				const sess = sessions.get( sessionId );
				const result = sess.applyRule( payload.rule, payload.word );
				parentPort.postMessage(
					{
						type: "RESULT",
						sessionId,
						result
					}
				);
			} 
			else if ( type === "APPLY_MANY" )
			{
				const sess = sessions.get( sessionId );
				const result = sess.applyRule( payload.rules, payload.word );
				parentPort.postMessage(
					{
						type: "RESULT",
						sessionId,
						result
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

