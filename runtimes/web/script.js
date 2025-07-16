var instance
const importObject = {
  env: {
    STACKTOP: 0,
    STACK_MAX: 65536,
    abortStackOverflow: function (val) { throw new Error("stackoverfow"); },
    memory: new WebAssembly.Memory({ initial: (1 << 16) }),
    nux_os_log: function (userdata, level, data, len) {
      const decoder = new TextDecoder();
      const str = decoder.decode(new Int8Array(instance.exports.memory.buffer, data, len))
      console.log(str);
      return 1;
    },
    nux_os_stats_update: ud => { return 1; },
    nux_os_input_update: ud => { return 1; },
    nux_os_buffer_create: ud => { return 1; },
    nux_os_buffer_update: ud => { return 1; },
    nux_os_texture_create: ud => { return 1; },
    nux_os_texture_update: ud => { return 1; },
    nux_os_framebuffer_create: ud => { return 1; },
    nux_os_create_pipeline: ud => { return 1; },
    nux_os_gpu_submit: ud => { return 1; },
    nux_os_file_open: ud => { return 1; },
    nux_os_file_seek: ud => { return 1; },
    nux_os_file_read: ud => { return 1; },
    nux_os_file_stat: ud => { return 1; },
  },
  wasi_snapshot_preview1: {
    environ_get: (environ, environBuf) => { return 0; },
    proc_exit: rval => { return 0; },
    fd_close: fd => { return 0; },
    fd_seek: (fd, offset_low, offset_high, whence, newOffset) => { return 0; },
    fd_write(fd, iovsPtr, iovsLength, bytesWrittenPtr) { return 0; },
    fd_fdstat_get: (fd, stat) => { return 0; }
  }
}
const runWasm = async () => {
  fetch("nux.wasm")
    .then(bytes => bytes.arrayBuffer())
    .then(mod => WebAssembly.compile(mod))
    .then(module => {
      console.log(WebAssembly.Module.imports(module))
      console.log(WebAssembly.Module.exports(module))
      // let instance = new WebAssembly.Instance(module, {});
      // console.log(instance.exports.square(15));
      console.log("ok")
    })
  // WebAssembly.instantiateStreaming(fetch("nux.wasm"), importObject).then(
  //   (obj) => {
  //     instance = obj.instance

  //     // let structMem = instance.exports.malloc(64);
  //     // console.log(structMem)
  //     // instance.exports.free(structMem)

  //     // Initialize instance
  //     instance.exports.start();
  //   },
  // );

  const canvas = document.querySelector("#gl-canvas");
  // Initialize the GL context
  const gl = canvas.getContext("webgl");

  // Only continue if WebGL is available and working
  if (gl === null) {
    alert(
      "Unable to initialize WebGL. Your browser or machine may not support it.",
    );
    return;
  }

  // Set clear color to black, fully opaque
  gl.clearColor(0.0, 0.0, 0.0, 1.0);
  // Clear the color buffer with specified clear color
  gl.clear(gl.COLOR_BUFFER_BIT);
};
runWasm();
