package main

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
)

type Sensors struct {
	Temperature  float64 `json:"temperature"`
	Illumination float64 `json:"illumination"`
	Pressure     float64 `json:"pressure"`
	Humidity     float64 `json:"humidity"`
	SoilMoisture int     `json:"soilMoisture"`
}

var sensors = Sensors{
	Temperature:  25.10,
	Illumination: 185.83,
	Pressure:     101.49,
	Humidity:     51.00,
	SoilMoisture: 107,
}

func getSensors(w http.ResponseWriter, r *http.Request) {
	json.NewEncoder(w).Encode(sensors)
}

func updateSensors(w http.ResponseWriter, r *http.Request) {
	body, err := ioutil.ReadAll(r.Body)
	var ans Sensors
	err = json.Unmarshal(body, &ans)
	if err != nil {
		panic(err)
	}
	sensors = ans
	log.Println(ans)
	return

}

func handleRequests() {
	getSensorsHandler := http.HandlerFunc(getSensors)
	updateSensorsHandler := http.HandlerFunc(updateSensors)
	http.HandleFunc("/sensors", getSensorsHandler)
	http.HandleFunc("/sensors/update", updateSensorsHandler)
	http.ListenAndServe(":8083", nil)
}

func main() {
	handleRequests()
}
