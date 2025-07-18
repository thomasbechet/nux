var instance
const decoder = new TextDecoder()
const decodeString = (data, len) => decoder.decode(new Int8Array(instance.exports.memory.buffer, data, len))
const importObject = {
  env: {
    STACKTOP: 0,
    STACK_MAX: 65536,
    abortStackOverflow: function (val) { throw new Error("stackoverfow"); },
    memory: new WebAssembly.Memory({ initial: (1 << 16) }),
    nux_os_log: function (userdata, level, data, len) {
      console.log(decodeString(data, len));
      return 1;
    },
    nux_os_file_open: (userdata, slot, path, len) => {
      console.log("open", decodeString(path, len));
      return 0;
    },
    nux_os_file_seek: (userdata, slot, n) => {
      console.log("seek", slot);
      return 1;
    },
    nux_os_file_read: (userdata, slot, p, n) => {
      console.log("read", p, n);
      return 1;
    },
    nux_os_file_stat: (userdata, slot, pstat) => {
      console.log("file stat", slot, pstat);
      return 1;
    },
    nux_os_pipeline_create: (userdata, slot, pinfo) => { return 1; },
    nux_os_framebuffer_create: (userdata, slot, texture) => { return 1; },
    nux_os_texture_create: (userdata, slot, pinfo) => { return 1; },
    nux_os_texture_update: (userdata, slot, x, y, w, h, pdata) => { return 1; },
    nux_os_buffer_create: (userdata, slot, type, size) => { return 1; },
    nux_os_buffer_update: (userdata, slot, offset, size, data) => { return 1; },
    nux_os_gpu_submit: (userdata, pcmds, count) => { return 1; },
    nux_os_input_update: (userdata, pbuttons, paxis) => { return 1; },
    nux_os_stats_update: (userdata, pstats) => { return 1; },

  },
  wasi_snapshot_preview1: {
    fd_close: fd => { return 0; },
    fd_read: fd => { return 0; },
    fd_seek: (fd, offset_low, offset_high, whence, newOffset) => { return 0; },
    fd_write(fd, iovsPtr, iovsLength, bytesWrittenPtr) { return 0; },
    fd_fdstat_get: (fd, stat) => { return 0; },
  }
}
const run = async () => {
  // fetch("nux.wasm")
  //   .then(bytes => bytes.arrayBuffer())
  //   .then(mod => WebAssembly.compile(mod))
  //   .then(module => {
  //     console.log(WebAssembly.Module.imports(module))
  //     console.log(WebAssembly.Module.exports(module))
  //     // let instance = new WebAssembly.Instance(module, {});
  //     // console.log(instance.exports.square(15));
  //     console.log("ok")
  //   })
  WebAssembly.instantiateStreaming(fetch("nux.wasm"), importObject).then(
    (obj) => {
      // Keep reference to instance
      instance = obj.instance
      // Initialize instance
      instance.exports.start();
    },
  );

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
  gl.clearColor(0.2, 0.0, 0.0, 1.0);
  // Clear the color buffer with specified clear color
  gl.clear(gl.COLOR_BUFFER_BIT);
};
run();
