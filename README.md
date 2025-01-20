# Retry

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

## Description

This is a header-only library that provides a simple and efficient way to retry functions based on their return values. The API is straightforward: users pass the target function, a list of values that will trigger a retry if the return value matches, the maximum number of attempts or retry period, and the list of arguments originally passed to their function.

## Features

- Supports retrying various function types, including free functions, static member functions, `std::function` objects, and anonymous lambdas.
- Handles both value and reference return types.
- Allows customization of the maximum retry period or the number of retry attempts.
- Provides options to set custom retry intervals.

## Usage

The `retry_until` and `retry_at_intervals_until` functions retry the user function until the return value no longer matches the specified list or the attempt/time limit is reached. Examples:

### Retry the function for 10 seconds
```c++
// User's function
Status join_room(const Member& member, int member_id) {
    // Function implementation
}

// Retry
Member user{"Alice"};
Status status = tsejhuang::retry_until(join_room, {Status::Busy, Status::MaxUsersReached}, std::chrono::seconds{10}, user, 1);
```

### Retry the function for 10 attempts
```c++
Status status = tsejhuang::retry_until(join_room, {Status::Busy, Status::MaxUsersReached}, 10, user, 1);
```

### Retry the function for 10 seconds with a 2-second interval
```c++
Status status = tsejhuang::retry_at_intervals_until(join_room, {Status::Busy, Status::MaxUsersReached}, std::chrono::seconds{10}, std::chrono::seconds{2}, user, 1);
```

### Retry the function for 10 attempts with a 2-second interval
```c++
Status status = tsejhuang::retry_at_intervals_until(join_room, {Status::Busy, Status::MaxUsersReached}, 10, std::chrono::seconds{2}, user, 1);
```

### Handling reference return types
If the return type of the user function is a reference, wrap each value in the list with `std::ref` or `std::cref`:

```c++
// User's function
const Member& get_current_speaker() {
    // Function implementation
}

// Retry
Member me{"Alice"};
Member other_user1{"Bob"};
Member other_user2{"Carol"};
const auto& speaker = tsejhuang::retry_at_intervals_until(get_current_speaker, {std::cref(other_user1), std::cref(other_user2)}, std::chrono::seconds{10}, std::chrono::seconds{2});

if (speaker != me) {
    // Retry failure
}
```

## API Documentation

See [retry.hpp](include/retry/retry.hpp) for API documentation and [tests](tests/retry.cpp) for example usage.

## Contributing

Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch: `git checkout -b feature-name`.
3. Commit your changes: `git commit -m "Add feature-name"`.
4. Push the branch: `git push origin feature-name`.
5. Submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
