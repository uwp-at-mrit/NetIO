#pragma once

#include "modbus/mbserver.hpp"
#include "modbus/codes.hpp"

namespace WarGrey::SCADA {
    private class ModbusVirtualDevice : public WarGrey::SCADA::IModbusServer {
    public:
        virtual ~ModbusVirtualDevice();
        
        ModbusVirtualDevice(WarGrey::SCADA::Syslog* logger, uint16 port = MODBUS_TCP_DEFAULT_PORT)
			: ModbusVirtualDevice(logger, 0, 0, 0, 0) {};
        
        ModbusVirtualDevice(WarGrey::SCADA::Syslog* logger,
			uint16 nbits, uint16 ninbits, uint16 nregisters, uint16 ninregisters, uint16 port = MODBUS_TCP_DEFAULT_PORT)
			: ModbusVirtualDevice(logger, 0, nbits, 0, ninbits, 0, nregisters, 0, ninregisters) {};

        ModbusVirtualDevice(WarGrey::SCADA::Syslog* logger,
			uint16 bit0, uint16 nbits, uint16 inbit0, uint16 ninbits,
            uint16 register0, uint16 nregisters, uint16 inregister0, uint16 ninregisters,
            uint16 port = MODBUS_TCP_DEFAULT_PORT);

    public:
        void initialize_discrete_inputs(uint16 idx, uint8 src);
        void initialize_discrete_inputs(uint16 idx, uint16 count, const uint8* src);
        void initialize_input_registers(uint16 idx, uint16 count, const uint16* src);

    public: // data access
		int read_coils(uint16 address, uint16 quantity, uint8* coil_status) override;
		int read_discrete_inputs(uint16 address, uint16 quantity, uint8* input_status) override;
		int write_coil(uint16 address, bool value) override;
        int write_coils(uint16 address, uint16 quantity, uint8* src) override;

		int read_holding_registers(uint16 address, uint16 quantity, uint8* register_values) override;
		int read_input_registers(uint16 address, uint16 quantity, uint8* input_registers) override;
		int write_register(uint16 address, uint16 value) override;
		int write_registers(uint16 address, uint16 quantity, uint8* src) override;

		int mask_write_register(uint16 address, uint16 and, uint16 or) override;
		int write_read_registers(uint16 waddr, uint16 wquantity, uint16 raddr, uint16 rquantity, uint8* rwpool) override;
		int read_queues(uint16 address, uint8* value_registers) override;

    public: // Diagnostics
		const char* access_private_device_identification(uint8 object) override;

    public: // Other

    private:
        uint16 bit0;
        uint16 nbits;
        uint8* coils;

    private:
        uint16 inbit0;
        uint16 ninbits;
        uint8* discrete_inputs;

    private:
        uint16 register0;
        uint16 nregisters;
        uint16* holding_registers;

    private:
        uint16 inregister0;
        uint16 ninregisters;
        uint16* input_registers;

    private:
        uint8* memory;
    };
}
