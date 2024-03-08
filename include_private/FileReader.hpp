#pragma once

#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Yo::File {

    template <class T>
    concept readable = requires(T a) {
        a.size();
        a.data();
    };

    template <template <class...> class Target, class T>
    struct is_template_of {
        static const bool value = false;
    };
    template <template <class...> class Target, class... Args>
    struct is_template_of<Target, Target<Args...>> {
        static const bool value = true;
    };

    template <readable T>
    struct MakeFunctional {
        explicit MakeFunctional(std::function<T&(size_t)> fn) :
        _fn(fn) {}
        explicit MakeFunctional(std::function<T&()> fn) :
        _fn([fn](size_t) -> T& { return fn(); }) {
        }
        std::function<T&(size_t)> _fn;
    };

    template <class T>
    concept is_read_func = is_template_of<MakeFunctional, T>::value;

    template <class T>
    size_t __Read(std::ifstream& file, T& args, [[maybe_unused]] size_t file_size) {
        file.read(reinterpret_cast<char*>(&args), sizeof(args));
        return sizeof(T);
    }

    template <readable T>
    size_t __Read(std::ifstream& file, T& args, [[maybe_unused]] size_t file_size) {
        if (args.data()) {
            file.read(reinterpret_cast<char*>(args.data()), args.size());
        } else {
            file.seekg(args.size(), std::ios::cur);
        }
        return args.size();
    }

    // 类型指引
    MakeFunctional(std::function<std::vector<uint8_t>&()>) -> MakeFunctional<std::vector<uint8_t>&>;
    MakeFunctional(std::function<std::vector<uint8_t>&(size_t)>) -> MakeFunctional<std::vector<uint8_t>&>;

    template <readable T>
    MakeFunctional(std::function<T&()>) -> MakeFunctional<T&>;
    template <readable T>
    MakeFunctional(std::function<T&(size_t)>) -> MakeFunctional<T&>;

    template <is_read_func T>
    size_t __Read(std::ifstream& file, T& args, size_t file_size) {
        auto& vector = args._fn(file_size);
        file.read(reinterpret_cast<char*>(vector.data()), vector.size());
        return vector.size();
    }

    struct SkipSize {
        SkipSize(size_t sz) :
        _size(sz) {}
        size_t _size = 0;
    };

    template <class T>
        requires std::is_same_v<std::decay_t<T>, SkipSize>
    size_t __Read(std::ifstream& file, T&& args, [[maybe_unused]] size_t file_size) {
        file.seekg(args._size, std::ios::cur);
        return args._size;
    }

    template <class... Args>
    bool ReadFile(const std::wstring& _fileName, Args&&... args) {
        std::ifstream file(_fileName, std::ios::binary | std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        file.seekg(0, std::ios::end);
        const auto file_size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        size_t size_of_read = 0;

        ([&size_of_read, &file, &args, file_size]() -> void {
            size_of_read += __Read(file, args, file_size);
        }(),
         ...);

        file.close();
        return size_of_read == file_size;
    }

} // namespace Yo::File
