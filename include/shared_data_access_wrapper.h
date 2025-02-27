#pragma once
#include <type_traits>


namespace std
{
    template<typename _Mutex>
    class unique_lock;
    template<typename _Mutex>
    class shared_lock;
    class shared_mutex;
}

namespace sdaw
{
    template<typename _LockType, typename _DataType>
    struct is_const_shared_data_access;

    template<typename _LockType, typename _DataType>
    constexpr bool is_const_shared_data_access_v = is_const_shared_data_access<_LockType, _DataType>::value;

    template<typename _LockType, typename _DataType>
    class shared_data_access_wrapper
    {
    public:

        static constexpr bool is_const_access = is_const_shared_data_access_v<_LockType, _DataType>;
        using data_reference = std::conditional_t<is_const_access, _DataType const&, _DataType&>;
        using data_pointer = std::conditional_t<is_const_access, _DataType const*, _DataType*>;

        shared_data_access_wrapper() = delete;

        shared_data_access_wrapper(data_reference data,
                                   typename _LockType::mutex_type& mutex)
            : data(data)
            , lock(mutex)
        {}

        shared_data_access_wrapper(shared_data_access_wrapper&& other)
            : data(other.data)
            , lock(std::move(other.lock))
        {}

        shared_data_access_wrapper(shared_data_access_wrapper const&) = delete;

        shared_data_access_wrapper& operator=(shared_data_access_wrapper&&) = delete;
        shared_data_access_wrapper& operator=(shared_data_access_wrapper const&) = delete;

        data_pointer operator->() const&& = delete;

        data_pointer operator->() const&
        {
            return &data;
        }

        data_reference operator*() const&& = delete;

        data_reference operator*() const&
        {
            return data;
        }

    private:

        data_reference data;

        _LockType lock;
    };

    template<typename _DataType>
    struct is_const_shared_data_access<std::shared_lock<std::shared_mutex>, _DataType>
    {
        static constexpr bool value = true;
    };

    template<typename _DataType>
    struct is_const_shared_data_access<std::unique_lock<std::shared_mutex>, _DataType>
    {
        static constexpr bool value = false;
    };

    template<typename _DataType>
    using shared_data_read_wrapper = shared_data_access_wrapper<std::shared_lock<std::shared_mutex>, _DataType>;
    template<typename _DataType>
    using shared_data_write_wrapper = shared_data_access_wrapper<std::unique_lock<std::shared_mutex>, _DataType>;
}
