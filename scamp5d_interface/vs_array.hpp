
#ifndef VS_ARRAY_HPP
#define VS_ARRAY_HPP

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>


template <typename _type_>
class vs_span{

public:
    _type_ first;
    _type_ last;

    vs_span(){
        first = 0;
        last = 0;
    }

    vs_span(const _type_ a,const _type_ b){
        if(a<b){
            first = a;
            last = b;
        }else{
            first = b;
            last = a;
        }
    }

    class iterator{

    public:
        _type_ i;

        iterator(_type_ init){
            i = init;
        }

        bool operator!=(iterator const& arg) const {
            return i != arg.i;
        }

        _type_ operator++(){
            return ++i;
        }

        const _type_& operator*() const { 
            return i; 
        }
    };

    inline auto begin(){
        return iterator(first);
    }

    inline auto end(){
        return iterator(last + 1);
    }

    inline auto begin() const{
        return iterator(first);
    }

    inline auto end() const{
        return iterator(last + 1);
    }


};


template <typename _type_>
class vs_array{

protected:

    size_t cols;
    size_t rows;
    _type_ *data_mem;


public:

    vs_array(){
        cols = rows = 0;
        data_mem = NULL;
    }
    
    vs_array(size_t row_size,size_t col_size){
        cols = col_size;
        rows = row_size;
        data_mem = (_type_*)malloc(get_byte_size());
    }
    
    vs_array(const vs_array<_type_>&) = delete;
    
    ~vs_array(){
        if(data_mem){
            free(data_mem);
        }
    }

    inline bool is_null()const{
        return data_mem==NULL;
    }
    
    inline const _type_* c_ptr(size_t r=0,size_t c=0)const{
        return &data_mem[r*cols + c];
    }
    
    inline _type_ get(size_t r,size_t c)const{
        return data_mem[r*cols + c];
    }

    // get number of rows
    inline size_t n_rows()const{
        return rows;
    }

    // get number of columns
    inline size_t n_cols()const{
        return cols;
    }

    inline size_t get_row_size()const{
        return rows;
    }

    inline size_t get_col_size()const{
        return cols;
    }

    inline size_t get_byte_size()const{
        return rows*cols*sizeof(_type_);
    }
    
    inline _type_&operator()(size_t r,size_t c){
        return data_mem[r*cols + c];
    }

    inline const _type_&operator()(size_t r,size_t c)const{
        return data_mem[r*cols + c];
    }
    
    inline vs_array<_type_>& operator=(const vs_array<_type_>&that){
        resize(that.rows,that.cols);
        this->copy(that.data_mem,that.get_byte_size());
        return *this;
    }

    inline void move_to(vs_array<_type_>&dest){
        dest.capture(rows,cols,data_mem);
        this->release();
    }

    inline void copy(const _type_*source,size_t bytes){
        memcpy(data_mem,source,bytes);
    }

    inline void resize(size_t row_size,size_t col_size){
        cols = col_size;
        rows = row_size;
        if(data_mem){
            data_mem = (_type_*)realloc(data_mem,get_byte_size());
        }else{
            data_mem = (_type_*)malloc(get_byte_size());
        }
    }

    inline void capture(size_t row_size,size_t col_size,_type_*mem){
        if(data_mem){
            free(data_mem);
        }
        cols = col_size;
        rows = row_size;
        data_mem = mem;
    }
    
    inline _type_*release(){
        auto r = data_mem;
        data_mem = NULL;
        cols = 0;
        rows = 0;
        return r;
    }

    int save_csv(const char*filepath)const{
        std::ofstream fs(filepath,std::fstream::out);
        if(!fs.is_open()){
            return -1;
        }
        for(size_t r=0;r<rows;r++){
            size_t c;
            for(c=0;c<(cols-1);c++){
                fs << std::to_string(get(r,c)) << ',';
            }
            fs << std::to_string(get(r,c)) << std::endl;
        }
        fs.close();
        return 0;
    }

    inline vs_span<size_t> row_span()const{
        return vs_span<size_t>(0,rows - 1);
    }

    inline vs_span<size_t> col_span()const{
        return vs_span<size_t>(0,cols - 1);
    }

};


template <typename _type_>
class vs_rect{

public:
    vs_span<_type_> r_range;
    vs_span<_type_> c_range;

    vs_rect(const _type_*vec4):
        r_range(vec4[0],vec4[2]),
        c_range(vec4[1],vec4[3])
    {

    }
    
    vs_rect(const _type_*a,const _type_*b):
        r_range(a[0],b[0]),
        c_range(a[1],b[1])
    {

    }
    
    vs_rect(const _type_ r0,const _type_ c0,const _type_ r1,const _type_ c1):
        r_range(r0,r1),
        c_range(c0,c1)
    {

    }

};

#endif
