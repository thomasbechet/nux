const runWasm = async () => {
    fetch("square.wasm") 
   .then(bytes => bytes.arrayBuffer()) 
   .then(mod => WebAssembly.compile(mod)) 
   .then(module => { 
      let instance = new WebAssembly.Instance(module, {});
      console.log(instance.exports.square(15));
   })
};
runWasm();
