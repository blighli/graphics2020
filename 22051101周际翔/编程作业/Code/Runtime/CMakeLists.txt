cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
            Base.hpp 
            PlatformDefines.hpp
            Assert.hpp
            Base64.hpp
            Unicode.hpp
            Iterator.hpp
            MemoryUtils.hpp
            Hash.hpp
            Functional.hpp
            Object.hpp
            Interface.hpp
            Algorithm.hpp
            Atomic.hpp
            Runtime.hpp
            Vector.hpp
            HashSet.hpp
            HashMap.hpp
            String.hpp
            RingDeque.hpp
            Name.hpp
            Math.hpp
            Memory.hpp
            Path.hpp
            Time.hpp
            Blob.hpp
            TSAssert.hpp
            Module.hpp
            Result.hpp
            Thread.hpp
            VM.hpp
            TypeSystem.hpp
            ObjPtr.hpp
            TypeBuilder.hpp
            HashMultiMap.hpp
            HashMultiSet.hpp
            Debug.hpp

            Source/OS.hpp
            Source/HashTable.hpp
            Source/Base64.cpp
            Source/Unicode.cpp
            Source/Name.hpp
            Source/Name.cpp
            Source/Math/MathBase.hpp
            Source/Math/Simd.hpp
            Source/Math/SimdConvert.hpp
            Source/Math/SimdMatrix.hpp
            Source/Math/SimdMisc.hpp
            Source/Math/SimdVector.hpp
            Source/Math/Vector.hpp
            Source/Math/Matrix.hpp
            Source/Math/Transform.hpp
            Source/Math/Quaternion.hpp
            Source/Math/Color.hpp
            Source/Module.hpp
            Source/Module.cpp
            Source/Error.cpp
            Source/Memory.cpp
            Source/Time.cpp
            Source/Thread.cpp
            Source/Assert.cpp
            Source/Runtime.cpp
            Source/Object.cpp
            Source/Context.hpp
            Source/TypeSystem.hpp
            Source/TypeSystem.cpp
            Source/Types/PrimitiveType.hpp
            Source/Debug.cpp
            )

if (WIN32)
    set(SRC_FILES ${SRC_FILES} 
        Source/Platform/Windows/AtomicImpl.hpp
        Source/Platform/Windows/MiniWin.hpp
        Source/Platform/Windows/Assert.cpp
        Source/Platform/Windows/Memory.cpp
        Source/Platform/Windows/Thread.cpp
        Source/Platform/Windows/OS.cpp
        Source/Platform/Windows/Sync.cpp
        Source/Platform/Windows/Debug.cpp
        Source/Platform/Windows/Time.cpp
        Source/Platform/Windows/File.cpp
        Source/Platform/Windows/DLL.cpp)
        
endif()

if(UNIX)
    set(SRC_FILES ${SRC_FILES}
        Source/Platform/POSIX/AtomicImpl.hpp
        Source/Platform/POSIX/Assert.cpp
        Source/Platform/POSIX/Debug.cpp
        Source/Platform/POSIX/DLL.cpp
        Source/Platform/POSIX/Signal.hpp
        Source/Platform/POSIX/Mutex.hpp
        Source/Platform/POSIX/Semaphore.hpp
        Source/Platform/POSIX/Sync.cpp
        Source/Platform/POSIX/Memory.hpp
        Source/Platform/POSIX/Memory.cpp
        Source/Platform/POSIX/Errno.hpp
        Source/Platform/POSIX/Time.hpp
        Source/Platform/POSIX/Time.cpp
        Source/Platform/POSIX/File.hpp
        Source/Platform/POSIX/File.cpp
        Source/Platform/POSIX/Thread.hpp
        Source/Platform/POSIX/Thread.cpp
        Source/Platform/POSIX/Fiber.hpp
        Source/Platform/POSIX/Runtime.cpp)
endif()

if(LIB)
    add_library(Runtime STATIC ${SRC_FILES})
else()
    add_library(Runtime SHARED ${SRC_FILES})
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})
