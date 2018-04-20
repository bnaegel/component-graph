//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef OrderedQueue_h
#define OrderedQueue_h

#include <utility>
#include <functional>
#include <queue>
#include <vector>
#include <set>
#include <map>


namespace LibTIM {
/// Ordered Queue
/** This structure allow the use of ordered queue, it is templated to deal with any type.
  the order is integer and decreasing ( order=0 have more priority than order=1)
 **/
template<class T>
class OrderedQueue
{
	typedef std::map<int, std::queue<T>, std::less<int> > TorderedQueue;
	public:
	  /// Creates an empty ordered queue
		OrderedQueue()  { }
		~OrderedQueue()  { }

    // TorderedQueue is a std::map, so copy ctor, egality test are provided by std::map class


		/// add an element in OQ with specified order
		void put(int order, T _val) { m_oq[order].push(_val); }

		int size() {return m_oq.size();}


		/// get a element in OQueue
  	T get() { T val;
              typename  TorderedQueue::iterator itermap=m_oq.begin();
              std::queue<T> *q=&(itermap->second);
              val=q->front();
              q->pop();
              if (q->empty()) m_oq.erase(itermap);
              return val;
            }

		/// bool if OQueue is empty
  	bool empty() { return m_oq.empty();}

  private :
     TorderedQueue m_oq;
};

/// Ordered Queue with double priority
/** This structure allow the use of ordered queue, it is templated to deal with any type.
  the order is double and is decreasing ( order=0 have more priority than order=1)
 **/

template<class T>
class OrderedQueueDouble
{
	typedef std::map<double, std::queue<T>, std::less<double> > TorderedQueue;
	public:
	  /// Creates an empty ordered queue
		OrderedQueueDouble()  { }
		~OrderedQueueDouble()  { }

    // TorderedQueue is a std::map, so copy ctor, egality test are provided by std::map class


		/// add an element in OQ with specified order
		void put(double order, T _val) { m_oq[order].push(_val); }


		/// get a element in OQueue
  	T get() { T val;
              typename  TorderedQueue::iterator itermap=m_oq.begin();
              std::queue<T> *q=&(itermap->second);
              val=q->front();
              q->pop();
              if (q->empty()) m_oq.erase(itermap);
              return val;
            }

		/// bool if OQueue is empty
  	bool empty() { return m_oq.empty();}

  private :
     TorderedQueue m_oq;
};


template<class T>
class Queue
{
  public:
  Queue()  { }

  /// add an element in Queue
  void put(T _t) { m_q.push(_t); }
  /// get a element in Queue
  T get() { T val=m_q.front(); m_q.pop(); return val; }
  /// bool if Queue is empty
  bool empty() { return m_q.empty();}

	private:
  std::queue<T> m_q;
};

}//end namespace

#endif
