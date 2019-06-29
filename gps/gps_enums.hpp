#pragma once

namespace WarGrey::SCADA {
	private enum class NMEA_PSMI : unsigned char { 
		Autonomouse, Differential, Estimated, Manual, NotValid, _
	};

	private enum class NMEA_GQI : unsigned char {
		Invalid, SinglePoint, PseudorangeDifferential,
		FixedRTK, FloatRTK,
		DeadReckoningMode, ManualInputMode, SimulatorMode,
		WAAS, _
	};
}
