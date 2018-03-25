#ifndef __LIST_H_
#define __LIST_H_

template<class T>
class List{
  public:
    List(){
      len = 0;
    }
  
    void append(T value){
      data[len] = value; 
      len += 1; 
    }
    
    T pop(int index){
      T value = data[index];
      for(int i=index+1;i<len;i++){
        data[i-1] = data[i];
      }
      len -= 1;
    }

    bool in(T value){
      for(int i=0;i<len;i++){
        if(data[i] == value){
          return true;
        }
      }
      return false;
    }

    int index(T value){
      // Returns first index of value
      for(int i=0;i<len;i++){
        if(data[i] == value){
          return i;
        }
      }

      // Returns -1 if no value is found
      return -1;
    }
    
    T minimum(){
      // Gets minimum value from list
      T value = data[0];
      for(int i=0;i<len;i++){
        if(data[i] < value){
          value = data[i];
        }
      }
      return value;
    }

    int getLength(){
      return len;
    }

    T operator[](int i) const {return data[i];}
    T & operator[](int i) {return data[i];}
    
  private:
    T data [20];
    int len;
     
};
#endif

