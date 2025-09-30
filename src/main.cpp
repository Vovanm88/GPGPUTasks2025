#include <CL/cl.h>
#include <libclew/ocl_init.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>



std::string dev_type_to_str(cl_device_type value){
	if(value == CL_DEVICE_TYPE_CPU)
		return "CPU";
	else if(value == CL_DEVICE_TYPE_GPU)
		return "GPU";
	else if(value == CL_DEVICE_TYPE_ACCELERATOR)
		return "ACCELERATOR";
	else if(value == CL_DEVICE_TYPE_DEFAULT)
		return "DEFAULT";
	else if(value == CL_DEVICE_TYPE_ALL)
		return "ALL";
	else
		return "UNKNOWN";
}

template<typename T>
std::string to_string(T value)
{
	std::ostringstream ss;
	ss << value;
	return ss.str();
}

void reportError(cl_int err, const std::string &filename, int line)
{
	if(CL_SUCCESS == err)
		return;

	// Таблица с кодами ошибок:
	// libs/clew/CL/cl.h:103
	// P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
	std::string message = "OpenCL error code " + to_string(err) + " encountered at " + filename + ":" + to_string(line);
	throw std::runtime_error(message);
}


#define OCL_SAFE_CALL(expr) reportError(expr, __FILE__, __LINE__)

int main()
{
	// Пытаемся слинковаться с символами OpenCL API в runtime (через библиотеку libs/clew)
	if(!ocl_init())
		throw std::runtime_error("Can't init OpenCL driver!");

	// Откройте
	// https://www.khronos.org/registry/OpenCL/sdk/1.2/docs/man/xhtml/
	// Нажмите слева: "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformIDs"
	// Прочитайте документацию clGetPlatformIDs и убедитесь, что этот способ узнать, сколько есть платформ, соответствует документации:
	cl_uint platformsCount = 0;
	OCL_SAFE_CALL(clGetPlatformIDs(0, nullptr, &platformsCount));
	std::cout << "Number of OpenCL platforms: " << platformsCount << std::endl;

	// Тот же метод используется для того, чтобы получить идентификаторы всех платформ - сверьтесь с документацией, что это сделано верно:
	std::vector<cl_platform_id> platforms(platformsCount);
	OCL_SAFE_CALL(clGetPlatformIDs(platformsCount, platforms.data(), nullptr));

	for(int platformIndex = 0; platformIndex < platformsCount; ++platformIndex)
	{
		std::cout << "Platform #" << (platformIndex + 1) << "/" << platformsCount << std::endl;
		cl_platform_id platform = platforms[platformIndex];

		// Откройте документацию по "OpenCL Runtime" -> "Query Platform Info" -> "clGetPlatformInfo"
		// Не забывайте проверять коды ошибок с помощью макроса OCL_SAFE_CALL
		size_t platformNameSize = 0;
		try{
			//OCL_SAFE_CALL(clGetPlatformInfo(platform, 69, 0, nullptr, &platformNameSize));
			OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &platformNameSize));		}
		catch(const std::runtime_error &exc)
		{
			std::cerr << exc.what() << std::endl;
		}
				
		// TODO 1.1
		// Попробуйте вместо CL_PLATFORM_NAME передать какое-нибудь случайное число - например 239
		// Т.к. это некорректный идентификатор параметра платформы - то метод вернет код ошибки
		// Макрос OCL_SAFE_CALL заметит это, и кинет ошибку с кодом
		// Откройте таблицу с кодами ошибок:
		// libs/clew/CL/cl.h:103 // тут должно быть очевидно 179 :)
		// P.S. Быстрый переход к файлу в CLion: Ctrl+Shift+N -> cl.h (или даже с номером строки: cl.h:103) -> Enter
		// Найдите там нужный код ошибки и ее название
		// Затем откройте документацию по clGetPlatformInfo и в секции Errors найдите ошибку, с которой столкнулись
		// в документации подробно объясняется, какой ситуации соответствует данная ошибка, и это позволит, проверив код, понять, чем же вызвана данная ошибка (некорректным аргументом param_name)
		// Обратите внимание, что в этом же libs/clew/CL/cl.h файле указаны всевоможные defines, такие как CL_DEVICE_TYPE_GPU и т.п.



		// TODO 1.2
		// Аналогично тому, как был запрошен список идентификаторов всех платформ - так и с названием платформы, теперь, когда известна длина названия - его можно запросить:
		std::vector<unsigned char> platformName(platformNameSize, 0);
		
		OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_NAME, platformNameSize, platformName.data(), nullptr));
		
		std::cout << "\tPlatform name: " << platformName.data() << std::endl;

		// TODO 1.3
		// Запросите и напечатайте так же в консоль вендора данной платформы
		size_t vendorSize = 0;
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0, nullptr, &vendorSize));

        std::vector<unsigned char> vendor(vendorSize, 0);
        OCL_SAFE_CALL(clGetPlatformInfo(platform, CL_PLATFORM_VENDOR,
                                        vendorSize, vendor.data(), nullptr));
		
        std::cout << "\tPlatform vendor: " << vendor.data() << std::endl;
		// TODO 2.1
		// Запросите число доступных устройств данной платформы (аналогично тому, как это было сделано для запроса числа доступных платформ - см. секцию "OpenCL Runtime" -> "Query Devices")
		cl_uint devicesCount = 0;
		
		OCL_SAFE_CALL(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &devicesCount));
        std::cout << "\tDevices on platform: " << devicesCount << std::endl;

        std::vector<cl_device_id> devices(devicesCount);
        OCL_SAFE_CALL(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, devicesCount, devices.data(), nullptr));

		for(int deviceIndex = 0; deviceIndex < devicesCount; ++deviceIndex)
		{
			// TODO 2.2
			// Запросите и напечатайте в консоль:
			// - Название устройства
			// - Тип устройства (видеокарта/процессор/что-то странное)
			// - Размер памяти устройства в мегабайтах
			// - Еще пару или более свойств устройства, которые вам покажутся наиболее интересными
			std::cout << "\tDevice #" << deviceIndex+1 << "/" << devicesCount << std::endl;

			size_t deviceNameSize = 0;
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_NAME, 0, nullptr, &deviceNameSize));
			std::vector<unsigned char> deviceName(deviceNameSize, 0);
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_NAME, deviceNameSize, deviceName.data(), nullptr));
			std::cout << "\t\tDevice name: " << deviceName.data() << std::endl;

			size_t deviceTypeSize = 0;
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_TYPE, 0, nullptr, &deviceTypeSize));
			cl_device_type deviceType = CL_DEVICE_TYPE_DEFAULT;
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_TYPE, deviceTypeSize, &deviceType, nullptr));
			std::cout << "\t\tDevice type: " << dev_type_to_str(deviceType) << std::endl;

			cl_ulong deviceGlobalMemorySize = 0;
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &deviceGlobalMemorySize, nullptr));
			std::cout << "\t\tDevice memory size: " << deviceGlobalMemorySize / (1024*1024)<<"MB" << std::endl;

			cl_uint deviceGlobalCacheLineSize = 0;
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &deviceGlobalCacheLineSize, nullptr));
			std::cout << "\t\tDevice memory cache line in bytes: " << deviceGlobalCacheLineSize << std::endl;

			 	
			cl_uint deviceMaxFreq = 0;
			OCL_SAFE_CALL(clGetDeviceInfo(devices[deviceIndex], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &deviceMaxFreq, nullptr));
			std::cout << "\t\tDevice max clock frequency in MHz: " << deviceMaxFreq << std::endl;
		}
	}

	return 0;
}
