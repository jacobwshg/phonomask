
import { parentPort } from "worker_threads";

import { fileURLToPath } from "url";
import { dirname } from "path";

const __filename = fileURLToPath( import.meta.url );
let __dirname;
try
{
	__dirname = dirname( __filename );
}
catch ( e )
{
	__dirname = process.cwd();
}

// for serving wasm files from cloud
//const wasmModulePath = process.env.WASM_MODULE_PATH || `${__dirname}/phmask.js`;
//const createPhmaskModule = await import( wasmModulePath );

console.log( "__dirname: ", __dirname );

let phmask = null;
const sessions = new Map();


///*
// Initialize the Wasm module within the worker
import createPhmaskModule from "./phmask.js";
createPhmaskModule().then(
	( module ) =>
	{
		console.log( "module: ", module );
	 		phmask = module;
		console.log( "worker initialized wasm module" );
		parentPort.postMessage(
			{ type: "READY" }
		);
	}
);
//*/

/*
// dynamic import - didn't work;
// module:  [Module: null prototype] { default: [AsyncFunction: Module] }

let wasmModulePath = process.env.WASM_MODULE_PATH || `${__dirname}/phmask.js`;

console.log( "wasmModulePath: ", wasmModulePath );
await import( wasmModulePath ).then(
	( module ) => {
		console.log( "module: ", module );
		phmask = module.default();
		console.log("worker initialized wasm module");
		parentPort.postMessage(
			{ type: "READY" }
		);
	}
).catch((err) => {
	console.error(`Failed to load WASM module from ${wasmModulePath}:`, err);
	parentPort.postMessage({
		type: "ERROR",
		error: `Failed to initialize WASM: ${err.message}`
	});
});
*/

/* listen for msg from parent */
parentPort.on(
	"message", 
	( msg ) => 
	{
		const { type, sessionId, worker_payload } = msg;

		try
		{
			console.log( `worker received msg type ${type}` );

			if ( type === "CREATE_SESSION" )
			{
				const sess = new phmask.PhmaskSession();
				console.log( "worker initialized wasm session" );
				const { table_str } = worker_payload;

				sess.populate( table_str );

				console.log( "worker wasm session populated" );
				// add session to pool
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
				// retrieve session
				const sess = sessions.get( sessionId );
				const { rule, word } = worker_payload;
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
				let { rules, word } = worker_payload;
				console.log( `worker received rules` );
				for ( let r of rules )
				{
					console.log( "\t", r );
				}
				console.log( `, word ${ word }` );
				let results = [];
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
			else if ( type === "FEATURES_STR" )
			{
				// retrieve session
				const sess = sessions.get( sessionId );
				const { segment } = worker_payload;
				console.log( `worker received segment ${segment}` );
				const result = sess.features_str( segment );
				console.log( `worker features_str result: ${result}` );
				parentPort.postMessage(
					{
						type: "RESULT",
						sessionId: sessionId,
						result: result
					}
				);
			}
			else if ( type === "DELETE_SESSION" )
			{
				console.log( "worker in delete session msg type" );
				const sess = sessions.get( sessionId );
				if ( sess )
				{
					console.log( "worker found session to delete" );
					sess.delete();
					sessions.delete( sessionId );
				}
				msg = `worker deleted session ${sessionId}`;
				console.log( msg );
				parentPort.postMessage(
					{
						type: "RESULT",
						sessionId: sessionId,
						result: msg,
					}
				);
			}
		}
		catch ( err )
		{
			console.log( "worker error: ", err );
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

