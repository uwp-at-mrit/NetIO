#include "mrit/message.hpp"
#include "box.hpp"

using namespace WarGrey::SCADA;

using namespace Windows::Storage::Streams;

inline static unsigned int foldsum(uint8* data, size_t size) {
	unsigned int checksum = 0;
	
	for (unsigned int i = 0; i < size; i++) {
		checksum += data[i];
	}

	return checksum;
}

inline static unsigned int foldsum(size_t value, size_t size) {
	uint8* data = (uint8*)&value;

	switch (size) {
	case 1: { uint8  v = (uint8)value;  data = (uint8*)&v; }; break;
	case 2: { uint16 v = (uint16)value; data = (uint8*)&v; }; break;
	case 4: { uint32 v = (uint32)value; data = (uint8*)&v; }; break;
	case 8: { uint64 v = (uint64)value; data = (uint8*)&v; }; break;
	default: break;
	}

	return foldsum(data, size);
}

inline static unsigned long long read_integer(IDataReader^ mrin, size_t size) {
	unsigned long long v = 0;

	switch (size) {
	case 1: v = mrin->ReadByte(); break;
	case 2: v = mrin->ReadUInt16(); break;
	case 4: v = mrin->ReadUInt32(); break;
	case 8: v = mrin->ReadUInt64(); break;
	default: break;
	}

	return v;
}

inline static unsigned int write_integer(IDataWriter^ mrout, size_t value, size_t size) {
	switch (size) {
	case 1: mrout->WriteByte((uint8)value); break;
	case 2: mrout->WriteUInt16((uint16)value); break;
	case 4: mrout->WriteUInt32((uint32)value); break;
	case 8: mrout->WriteUInt64((uint64)value); break;
	default: break;
	}

	return foldsum(value, size);
}

/*************************************************************************************************/
MrMessageConfiguration::MrMessageConfiguration(size_t dball, size_t alignment_size, size_t old_protocol_data_size) {
	this->set_header(0x24U, 1U); // '$'

	this->set_command_size(1U);
	this->set_datablock_slot_size(2U);
	this->set_start_address_size(2U);
	this->set_end_address_size(2U);
	this->set_datasize_size(2U);
	this->set_checksum_size(2U);

	this->set_tail(0x0D0AU, 2U); // Carriage Return + Line Feed

	this->set_fcode();
	this->set_alignment_size(alignment_size);

	this->db_read_all = dball;
	this->old_protocol_data_size = old_protocol_data_size;
}

bool MrMessageConfiguration::is_old_protocol() {
	return (this->old_protocol_data_size > 0);
}

void MrMessageConfiguration::set_fcode(char read_signal, char write_analog_quantity, char write_digital_quantity) {
	this->read_signal = read_signal;
	this->write_analog_quantity = write_analog_quantity;
	this->write_digital_quantity = write_digital_quantity;
}

char MrMessageConfiguration::read_signal_fcode() {
	return this->read_signal;
}

char MrMessageConfiguration::write_analog_quantity_fcode() {
	return this->write_analog_quantity;
}

char MrMessageConfiguration::write_digital_quantity_fcode() {
	return this->write_digital_quantity;
}

size_t MrMessageConfiguration::read_all_dbcode() {
	return this->db_read_all;
}

size_t MrMessageConfiguration::predata_size() {
	size_t total = this->header_size + this->fcode_size;

	if (!this->is_old_protocol()) {
		total += (this->dbid_size + this->addr0_size + this->addrn_size + this->datasize_size);
	}

	return total;
}

size_t MrMessageConfiguration::postdata_size() {
	size_t total = this->tail_size;

	if (!this->is_old_protocol()) {
		total += this->checksum_size;
	}

	return total;
}

size_t MrMessageConfiguration::read_header(IDataReader^ mrin, size_t* head, size_t* fcode, size_t* db_id, size_t* addr0, size_t* addrn, size_t* size) {
	(*head)  = (size_t)read_integer(mrin, this->header_size);
	(*fcode) = (size_t)read_integer(mrin, this->fcode_size);

	if (this->is_old_protocol()) {
		(*db_id) = this->db_read_all;
		(*addr0) = 0;
		(*addrn) = this->old_protocol_data_size;
		(*size) = this->old_protocol_data_size;
	} else {
		(*db_id) = (size_t)read_integer(mrin, this->dbid_size);
		(*addr0) = (size_t)read_integer(mrin, this->addr0_size);
		(*addrn) = (size_t)read_integer(mrin, this->addrn_size);
		(*size) = (size_t)read_integer(mrin, this->datasize_size);
	}

	return (*size) + this->postdata_size();
}

void MrMessageConfiguration::read_body_tail(IDataReader^ mrin, size_t size, uint8* data, size_t* checksum, size_t* eom) {
	READ_BYTES(mrin, data, size);

	if (!this->is_old_protocol()) {
		(*checksum) = (size_t)read_integer(mrin, this->checksum_size);
	}

	(*eom) = (size_t)read_integer(mrin, this->tail_size);
}

void MrMessageConfiguration::write_header(IDataWriter^ mrout, size_t fcode, size_t db_id, size_t addr0, size_t addrn) {
	this->header_checksum
		= write_integer(mrout, this->header_value, this->header_size)
		+ write_integer(mrout, fcode, this->fcode_size)
		+ write_integer(mrout, db_id, this->dbid_size)
		+ write_integer(mrout, addr0, this->addr0_size)
		+ write_integer(mrout, addrn, this->addrn_size);
}

void MrMessageConfiguration::write_body_tail(IDataWriter^ mrout, uint8* data, size_t size) {
	uint16 checksum = ((this->header_checksum + foldsum(size, this->datasize_size) + foldsum(data, size)) & 0xFFFF);

	write_integer(mrout, size, this->datasize_size);
	WRITE_BYTES(mrout, data, size);
	write_integer(mrout, checksum, this->checksum_size);
	write_integer(mrout, this->tail_value, this->tail_size);
}

void MrMessageConfiguration::write_aligned_tail(IDataWriter^ mrout, uint8* data, size_t size) {
	size_t padding_start = this->predata_size() + size + this->postdata_size();
	
	this->write_body_tail(mrout, data, size);
	for (size_t i = padding_start; i < this->alignment_size; i++) {
		mrout->WriteByte(0xFF);
	}
}

void MrMessageConfiguration::set_alignment_size(size_t size) {
	this->alignment_size = size;
}

void MrMessageConfiguration::set_header(size_t value, size_t size) {
	this->header_value = value;
	this->header_size = size;
}

void MrMessageConfiguration::set_tail(size_t value, size_t size) {
	this->tail_value = value;
	this->tail_size = size;
}

bool MrMessageConfiguration::header_match(size_t value, size_t* expected) {
	SET_BOX(expected, this->header_value);

	return (this->header_value == value);
}

bool MrMessageConfiguration::tail_match(size_t value, size_t* expected) {
	SET_BOX(expected, this->tail_value);

	return (this->tail_value == value);
}

void MrMessageConfiguration::set_command_size(size_t size) {
	this->fcode_size = size;
}

void MrMessageConfiguration::set_datablock_slot_size(size_t size) {
	this->dbid_size = size;
}
void MrMessageConfiguration::set_start_address_size(size_t size) {
	this->addr0_size = size;
}

void MrMessageConfiguration::set_end_address_size(size_t size) {
	this->addrn_size = size;
}

void MrMessageConfiguration::set_datasize_size(size_t size) {
	this->datasize_size = size;
}

void MrMessageConfiguration::set_checksum_size(size_t size) {
	this->checksum_size = size;
}
