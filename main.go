package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strconv"
	"strings"
	"time"
)

type Sensors struct {
	Temperature  float32 `json:"temperature"`
	Illumination float32 `json:"illumination"`
	Pressure     float32 `json:"pressure"`
	Humidity     float32 `json:"humidity"`
	SoilMoisture int     `json:"soilMoisture"`
	Time         string  `json:"time"`
}

type SensorsList struct {
	Result []Sensors `json:"result"`
}

var sensors = Sensors{
	Temperature:  25.10,
	Illumination: 185.83,
	Pressure:     101.49,
	Humidity:     51.00,
	SoilMoisture: 107,
	Time:         "24",
}

func getSensors(w http.ResponseWriter, r *http.Request) {
	json.NewEncoder(w).Encode(sensors)
}

func getSensorsList(w http.ResponseWriter, r *http.Request) {
	dat, err := os.ReadFile("sensors_list.txt")
	if err != nil {
		panic(err)
	}
	sas := strings.Split(string(dat), "\n")
	var ans = SensorsList{Result: make([]Sensors, len(sas)-1)}
	for i := 0; i < len(sas)-1; i++ {
		s := strings.Split(sas[i], ",")
		temp, err := strconv.ParseFloat(s[0], 32)
		if err != nil {
			panic(err)
		}
		ill, err := strconv.ParseFloat(s[1], 32)
		if err != nil {
			panic(err)
		}
		press, err := strconv.ParseFloat(s[2], 32)
		if err != nil {
			panic(err)
		}
		hum, err := strconv.ParseFloat(s[3], 32)
		if err != nil {
			panic(err)
		}
		soil, err := strconv.ParseInt(s[4], 0, 64)
		if err != nil {
			panic(err)
		}
		t := s[5]
		ans.Result[i] = Sensors{
			Temperature:  float32(temp),
			Illumination: float32(ill),
			Pressure:     float32(press),
			Humidity:     float32(hum),
			SoilMoisture: int(soil),
			Time:         t,
		}
	}
	json.NewEncoder(w).Encode(ans)
	log.Println(ans)
}

func updateSensors(w http.ResponseWriter, r *http.Request) {
	body, err := ioutil.ReadAll(r.Body)
	var ans Sensors
	err = json.Unmarshal(body, &ans)
	if err != nil {
		panic(err)
	}
	sensors = ans
	dt := time.Now()
	writeIntoFile(fmt.Sprintf("%f,%f,%f,%f,%d,%s\n", ans.Temperature, ans.Illumination, ans.Pressure, ans.Humidity, ans.SoilMoisture, dt.Format("01-02-2006T15:04:05Z")))
	log.Println(ans)
	return
}

func handleRequests() {
	getSensorsHandler := http.HandlerFunc(getSensors)
	updateSensorsHandler := http.HandlerFunc(updateSensors)
	getSensorsListHandler := http.HandlerFunc(getSensorsList)
	http.HandleFunc("/sensors", getSensorsHandler)
	http.HandleFunc("/sensors/list", getSensorsListHandler)
	http.HandleFunc("/sensors/update", updateSensorsHandler)
	http.ListenAndServe(":8083", nil)
}

func writeIntoFile(str string) {
	f, err := os.OpenFile("sensors_list.txt", os.O_APPEND|os.O_WRONLY|os.O_CREATE, 0600)
	defer f.Close()
	if err != nil {
		return
	}
	f.WriteString(str)
	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		log.Println(scanner.Text())
	}
}

func main() {
	handleRequests()
}
