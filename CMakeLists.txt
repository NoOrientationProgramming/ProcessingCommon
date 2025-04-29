
idf_component_register(
	SRCS
	"LibTime.cpp"
	"LibFilesys.cpp"
	"LibDspc.cpp"
	"ThreadPooling.cpp"
	"EspLedPulsing.cpp"
	INCLUDE_DIRS
	"."
	REQUIRES
	"SystemCore"
	PRIV_REQUIRES
	"driver"
)

