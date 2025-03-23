import express from "express";
import dotenv from "dotenv";
import cors from "cors";
dotenv.config();

const app = express();
app.use(cors());

let maxtemp = 0;
let pressure = 0;
const PORT = process.env.PORT || 5000;

app.use(express.json());

app.post('/data',(req, res) => {
    maxtemp = req.body?.maxtemp
    pressure = req.body?.pressure
    res.status(200).send({
        "status": "success"
      });
})

app.get('/data', (req, res)=>{
    res.send({maxtemp, pressure}).status(200)
})

app.get('/health', (req, res)=>{
    res.send({status: 'ok'}).status(200)
})

app.listen(PORT, ()=>{
    console.log(`Server is running on port: ${PORT} `)
})