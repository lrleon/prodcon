# ifndef ITEM_QUEUE_H
# define ITEM_QUEUE_H

# include <vector>
# include <queue>
# include <condition_variable>
# include <optional>

/** This is a thread safe queue of strings in which race conditions
    are avoided.

    Consumers threads extract strings by using `get()` method. If the
    queue is empty, the calling thread suspends until a new item is
    put through `put()` method or the queue has been marked as
    finished. `get()` return a C++-17 `optional<string>` type, which
    is useful because eventually a thread could be resumed because the
    queue has been marked as finished; in this case, the return value
    won't have value.

    Producers threads insert strings by using `put()` method, which
    eventually wakes up a suspended thread having performed `get()` on
    an empty queue.

    Once it is known that no more strings have to be inserted in the
    queue, then the queue can be marked as **finished** through the
    method `finish()`. It is guaranteed that if the return value of
    `get()` has not value, then queue is finished. So a resumed thread
    no having value can safely know that the production process has
    finished and eventually to finish. For performance reasons and
    because it would be a programming error, `put()` does not
    validates if the queue has been marked as finished.

    Threads can consult if the queue has been marked as finished by
    calling `is_finished()` method.
 */
class ItemQueue
{
  std::queue<std::string> q;
  bool finished = false; // true when no more items will be read into the queue
  std::mutex m;
  std::condition_variable cond_var;

public:

  // Return true if queue has been marked as finished and queue is
  // empty. This indicates that no more readings are needed
  bool is_finished() noexcept
  {
    std::unique_lock<std::mutex> lock(m);
    return finished and q.empty();
  }

  // Mark the queue as finished and signal and wake up all waiting
  // threads so that they could finish
  void finish() noexcept
  {
    std::unique_lock<std::mutex> lock(m);
    finished = true;
    cond_var.notify_all(); // wake up all suspended threads
  }

  /// Insert a string to the queue. Please note the move semantic,
  /// which eventually avoids string copy and frees the string
  /// memory. An overloaded version that perform a copy could be written
  /// if an application requires to keep the string
  void put(std::string && item) noexcept
  {
    std::unique_lock<std::mutex> lock(m);
    q.push(std::forward<std::string>(item));
    cond_var.notify_one(); // wake up a suspended thread that has
                           // performed get on an empty queue
  }

  /// Extract the oldest string form the queue and returns it as
  /// `optional<string>`. If the return value has not value, then the
  /// caller can conclude that the queue has been marked as finished
  /// and no item are pending for being extracted.
  std::optional<std::string> get() noexcept
  {
    std::unique_lock<std::mutex> lock(m);
    while (true)
      {
        if (not q.empty())
          {
            std::string item = move(q.front());
            q.pop();
            return item;
          }

        if (finished)
          return {};

        // in this case queue is empty but the writing in the queue
        // has not still finished. So we suspend and wait for
        // signaling when a item is put into the queue or the queue is
        // marked as finished
        cond_var.wait(lock);
      }
  }
};

# endif
