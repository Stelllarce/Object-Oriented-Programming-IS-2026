class Mock {
public:
    Mock(size_t s) : size(s), data(new int[n]{})
    ~Mock() { delete[] dyn_data; }

    Mock(const Mock& other);
    Mock& operator=(const Mock& other);

private:
    int* dyn_data;
    size_t size;
}