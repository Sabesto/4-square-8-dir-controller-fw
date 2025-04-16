#pragma once
#ifndef TCA9539_H
#define TCA9539_H

#include <Arduino.h>
#include <Wire.h>


class TCA9539 {
    uint8_t I2C_ADDR = 0x74;


    TwoWire* wire;
    uint8_t sts;

public:
    enum class Port { PORT0, PORT1 };
    enum class Reg { INPUT_PORT0, INPUT_PORT1, OUTPUT_PORT0, OUTPUT_PORT1, POLARITY0, POLARITY1, CONFIG0, CONFIG1 };
    enum class Config { OUT, IN };
    enum class Polarity { ORIGINAL, INVERSE };

    void attach(TwoWire& w) { wire = &w; }

    void setDeviceAddress(const uint8_t addr) { I2C_ADDR = addr; }

    uint8_t input(const Port port, const uint8_t pin);
    uint8_t input(const Port port);

    uint8_t  output(const uint8_t pin, const uint8_t state);
    uint8_t  output(const uint16_t state);
    uint8_t  outputPort(const Port port, const uint8_t state);
    uint16_t output();
    uint8_t  polarity(const Port port, const uint8_t pin, const Polarity pol);
    uint8_t  polarity(const Port port, const Polarity pol);
    uint8_t  polarity(const Port port);
    uint8_t  config(const Port port, const uint8_t pin, const Config cfg);
    uint8_t  config(const Port port, const Config cfg);
    uint8_t  config(const Port port);
    uint8_t  status() const { return sts; }

private:
    uint8_t readBit(uint8_t dev, uint8_t reg, uint8_t bit);
    uint8_t readByte(uint8_t dev, uint8_t reg);
    int8_t readBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t *data);
    bool writeBit(uint8_t dev, uint8_t reg, uint8_t bit, uint8_t data);
    bool writeByte(uint8_t dev, uint8_t reg, uint8_t data);
    bool writeBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t* data);
};

#endif // TCA9539_H_