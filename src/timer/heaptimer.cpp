/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */ 
#include "heaptimer.h"

/*--------------------------- public ---------------------------*/

void HeapTimer::adjust(int fd, int timeout) {
    /* 调整指定id的结点 */
    assert(!heap_.empty() && ref_.count(fd) > 0);
    heap_[ref_[fd]].expires = Clock::now() + MS(timeout);;
    siftdown_(ref_[fd], heap_.size());
}


void HeapTimer::add(int fd, int timeout, const TimeoutCallBack& cb) {
    assert(fd >= 0);
    size_t i;
    if(ref_.count(fd) == 0) {
        /* 新节点：堆尾插入，调整堆 */
        i = heap_.size();
        ref_[fd] = i;
        heap_.push_back({fd, Clock::now() + MS(timeout), cb});
        siftup_(i);
    } 
    else {
        /* 已有结点：调整堆 */
        i = ref_[fd];
        heap_[i].expires = Clock::now() + MS(timeout);
        heap_[i].cb = cb;
        if(!siftdown_(i, heap_.size())) {
            siftup_(i);
        }
    }
}


void HeapTimer::pop() {
    assert(!heap_.empty());
    del_(0);
}

void HeapTimer::clear() {
    ref_.clear();
    heap_.clear();
}


void HeapTimer::tick() {
    /* 清除超时结点 */
    if(heap_.empty()) {
        return;
    }
    while(!heap_.empty()) {
        TimerNode node = heap_.front();
        if(std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0) { 
            break; 
        }
        node.cb();
        pop();
    }
}

int HeapTimer::GetNextTick() {
    tick();
    size_t res = -1;
    if(!heap_.empty()) {
        res = std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now()).count();
        if(res < 0) { res = 0; }
    }
    return res;
}



// void HeapTimer::doWork(int id) {
//     /* 删除指定id结点，并触发回调函数 */
//     if(heap_.empty() || ref_.count(id) == 0) {
//         return;
//     }
//     size_t i = ref_[id];
//     TimerNode node = heap_[i];
//     node.cb();
//     del_(i);
// }


/*--------------------------- private ---------------------------*/

void HeapTimer::del_(size_t heap_id) {
    /* 删除指定位置的结点 */
    assert(!heap_.empty() && heap_id >= 0 && heap_id < heap_.size());
    /* 将要删除的结点换到队尾，然后调整堆 */
    size_t i = heap_id;
    size_t n = heap_.size() - 1;
    assert(i <= n);
    if(i < n) {
        SwapNode_(i, n);    
        if(!siftdown_(i, n)) {
            siftup_(i);
        }
    }
    /* 队尾元素删除 */
    ref_.erase(heap_.back().sockfd);
    heap_.pop_back();
}


//上滤操作
void HeapTimer::siftup_(size_t heap_id) {
    assert(heap_id >= 0 && heap_id < heap_.size());
    size_t i = heap_id;
    size_t j = (i - 1) / 2;
    while(j >= 0) {
        if(heap_[j] < heap_[i]) { break; }
        SwapNode_(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}

//下滤操作
bool HeapTimer::siftdown_(size_t heap_id, size_t n) {
    assert(heap_id >= 0 && heap_id < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    size_t i = heap_id;
    size_t j = i * 2 + 1;
    while(j < n) {
        if(j + 1 < n && heap_[j + 1] < heap_[j]) j++;    //j指向最小的子树
        if(heap_[i] < heap_[j]) break;    //如果父节点比子树小，已满足要求，则退出
        SwapNode_(i, j);
        i = j;
        j = i * 2 + 1;
    }
    return i > heap_id; //true表示下滤操作成功
}

void HeapTimer::SwapNode_(size_t i, size_t j) {
    assert(i >= 0 && i < heap_.size());
    assert(j >= 0 && j < heap_.size());
    std::swap(heap_[i], heap_[j]);
    ref_[heap_[i].sockfd] = i;
    ref_[heap_[j].sockfd] = j;
} 



