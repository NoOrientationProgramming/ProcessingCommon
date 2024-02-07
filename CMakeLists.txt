
idf_component_register(
	SRCS
	"ThreadPooling.cpp"
	"LibTime.cpp"
	"EspLedPulsing.cpp"
	INCLUDE_DIRS
	"."
	REQUIRES
	"ProcessingCore"
	PRIV_REQUIRES
	"driver"
)

