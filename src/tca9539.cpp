#include "tca9539.h"

uint8_t TCA9539::input(const Port port, const uint8_t pin) {
    if(port == Port::PORT0){
        return readBit(I2C_ADDR, (uint8_t)Reg::INPUT_PORT0, pin);
    } else if (port == Port::PORT1){
        return readBit(I2C_ADDR, (uint8_t)Reg::INPUT_PORT1, pin);
    } else {
        return -1;
    }
}

uint8_t TCA9539::input(const Port port) {
    if(port == Port::PORT0){
        return readByte(I2C_ADDR, (uint8_t)Reg::INPUT_PORT0);
    } else if (port == Port::PORT1){
        return readByte(I2C_ADDR, (uint8_t)Reg::INPUT_PORT1);
    } else {
        return -1;
    }
}

uint8_t TCA9539::output(const uint8_t pin, const uint8_t state) {
    uint8_t bit;
    uint8_t reg;
    if (pin <= 7) {
        bit = pin;
        reg = (uint8_t)Reg::OUTPUT_PORT0;
    } else if (pin > 7) {
        bit = pin - 8;
        reg = (uint8_t)Reg::OUTPUT_PORT1;
    }
    
    return writeBit(I2C_ADDR, reg, bit, (uint8_t)(state & 0x01));
}

uint8_t TCA9539::outputPort(const Port port, const uint8_t state) {
    uint8_t reg = (port == Port::PORT0) ? (uint8_t)Reg::OUTPUT_PORT0 : (uint8_t)Reg::OUTPUT_PORT1;
    return writeByte(I2C_ADDR, reg, state);   
}

uint8_t TCA9539::output(const uint16_t state) {
    return writeBytes(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT0, 2, (uint8_t*)&state);
}

uint16_t TCA9539::output() {
    uint16_t data = 0;
    data |= (uint16_t)(readByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT0) << 0);
    data |= (uint16_t)(readByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT1) << 8);
    return data;
}

uint8_t TCA9539::polarity(const Port port, const uint8_t pin, const Polarity pol)
{
    if(port == Port::PORT0){
        return writeBit(I2C_ADDR, (uint8_t)Reg::POLARITY0, pin, (uint8_t)pol);
    } else if (port == Port::PORT1){
        return writeBit(I2C_ADDR, (uint8_t)Reg::POLARITY1, pin, (uint8_t)pol);
    } else {
        return -1;
    }
}
uint8_t TCA9539::polarity(const Port port, const Polarity pol) {
    uint8_t d = (pol == Polarity::ORIGINAL) ? 0x00 : 0xFF;
    if(port == Port::PORT0){
        return writeByte(I2C_ADDR, (uint8_t)Reg::POLARITY0, (uint8_t)d);
    } else if (port == Port::PORT1){
        return writeByte(I2C_ADDR, (uint8_t)Reg::POLARITY1, (uint8_t)d);
    } else {
        return -1;
    }
}
uint8_t TCA9539::polarity(const Port port) {
    if(port == Port::PORT0){
        return readByte(I2C_ADDR, (uint8_t)Reg::POLARITY0);
    } else if (port == Port::PORT1){
        return readByte(I2C_ADDR, (uint8_t)Reg::POLARITY1);
    } else {
        return -1;
    }
}

uint8_t TCA9539::config(const Port port, const uint8_t pin, const Config cfg) {
    if(port == Port::PORT0){
        return writeBit(I2C_ADDR, (uint8_t)Reg::CONFIG0, pin, (uint8_t)cfg);
    } else if (port == Port::PORT1){
        return writeBit(I2C_ADDR, (uint8_t)Reg::CONFIG1, pin, (uint8_t)cfg);
    } else {
        return -1;
    }
}
uint8_t TCA9539::config(const Port port, const Config cfg) {
    uint8_t d = (cfg == Config::OUT) ? 0x00 : 0xFF;
    if(port == Port::PORT0){
        return writeByte(I2C_ADDR, (uint8_t)Reg::CONFIG0, (uint8_t)d);
    } else if (port == Port::PORT1){
        return writeByte(I2C_ADDR, (uint8_t)Reg::CONFIG1, (uint8_t)d);
    } else {
        return -1;
    }
}
uint8_t TCA9539::config(const Port port) {
    if(port == Port::PORT0){
        return readByte(I2C_ADDR, (uint8_t)Reg::CONFIG0);
    } else if (port == Port::PORT1){
        return readByte(I2C_ADDR, (uint8_t)Reg::CONFIG1);
    } else {
        return -1;
    }
}


uint8_t TCA9539::readBit(uint8_t dev, uint8_t reg, uint8_t bit){
        uint8_t b = readByte(dev, reg);
        b &= (1 << bit);
        return b;
    }

uint8_t TCA9539::readByte(uint8_t dev, uint8_t reg){
    uint8_t data;
    readBytes(dev, reg, 1, &data);
    return data;
}

int8_t TCA9539::readBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t *data) {
    wire->beginTransmission(dev);
    wire->write(reg);
    wire->endTransmission();
    wire->requestFrom(dev, size);
    int8_t count = 0;
    while (wire->available()) data[count++] = wire->read();
    return count;
}

bool TCA9539::writeBit(uint8_t dev, uint8_t reg, uint8_t bit, uint8_t data) {
    uint8_t b = readByte(dev, reg);
    b = (data != 0) ? (b | (1 << bit)) : (b & ~(1 << bit));
    return writeByte(dev, reg, b);
}

bool TCA9539::writeByte(uint8_t dev, uint8_t reg, uint8_t data) {
    return writeBytes(dev, reg, 1, &data);
}

bool TCA9539::writeBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t* data) {
    wire->beginTransmission(dev);
    wire->write(reg);
    for (uint8_t i = 0; i < size; i++)
        wire->write(data[i]);
    sts = wire->endTransmission();
    if (sts != 0) {
        Serial.print("I2C ERROR : ");
        Serial.println(sts);
    }
    return (sts == 0);
}