# ifndef ITEM_QUEUE_H
# define ITEM_QUEUE_H

# include <vector>
# include <queue>
# include <condition_variable>
# include <optional>

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
  }

  void put(std::string && item) noexcept
  {
    std::unique_lock<std::mutex> lock(m);
    q.push(std::forward<std::string>(item));
    cond_var.notify_one(); // wake up a waiting thread that has performed get
  }

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
        // signaling when a item is put into the queue or the writing
        // finishes
        cond_var.wait(lock);
      }
  }
};

# endif
