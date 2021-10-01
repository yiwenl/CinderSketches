const { Client, Server } = require('node-osc')

const PORT_LISTENING = 10001
const PORT_SENDING = 3333

// listeneing
const oscServer = new Server(PORT_LISTENING, '192.168.1.87', () => {
  console.log('Server is listening')
})

oscServer.on('/cameraPos', (msg) => {
  console.log('on camera :', msg)
  sendCameraPos(msg[1], msg[2], msg[3])
})

// sending
const client = new Client('127.0.0.1', PORT_SENDING)

const sendCameraPos = (x, y, z) => {
  client.send('/cameraPos', x, y, z, () => {
    // console.log('Done sending')
  })
}
