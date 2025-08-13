#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <chrono>
#include <ctime>


using namespace std;

int RANGE_SIZE = 500000;
int ARRAY_SIZE = 200000000;
// +- 1 gb;

void Swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int Partition(int*& arr, int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            Swap(&arr[i], &arr[j]);
        }
    }
    Swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(int*& arr, int low, int high)
{
    if (low < high)
    {
        int pivot = Partition(arr, low, high);
        quickSort(arr, low, pivot - 1);
        quickSort(arr, pivot + 1, high);
    }
}

void merge_to_file(const int* arr1, const int* arr2, int size1, int size2)
{
    fstream temp;
    const int* first;
    const int* second;

    temp.open("temp_1_file.txt", fstream::out | std::ofstream::trunc);

    if (arr1[0] < arr2[0])
    {
        first = arr1;
        second = arr2;
    }
    else
    {
        first = arr2;
        second = arr1;
        Swap(&size1, &size2);
    }

    if (temp.is_open())
    {
        int i = 0;
        int j = 0;

        while (i < size1 && j < size2)
        {
            if (first[i] == second[j])
            {
                temp << first[i++] << ' ';
            }
            else if (first[i] == second[j])
            {
                temp << first[i++] << ' ';
                temp << second[j++] << ' ';
            }
            else
            {
                temp << second[j++] << ' ';
            }
        }
        while (i < size1)
        {
            temp << first[i++] << ' ';
        }
        while (j < size2)
        {
            temp << second[j++] << ' ';
        }
        temp.close();
    }
}

void merge_file()
{
    fstream res;
    fstream temp_1;
    fstream temp_2;

    temp_1.open("temp_1_file.txt", fstream::in);
    res.open("res_file.txt", fstream::in);
    temp_2.open("temp_2_file.txt", fstream::out | fstream::trunc);

    if (!temp_1.is_open() || !temp_2.is_open() || res.is_open())
    {
        return;
    }

    int temp_1_value;
    int res_value;

    temp_1 >> temp_1_value;
    res >> res_value;
    while (!temp_1.eof() && !res.eof())
    {
        if (temp_1_value <= res_value)
        {
            temp_2 << temp_1_value << ' ';
            temp_1 >> temp_1_value;
        }
        else {
            temp_2 << res_value << ' ';
            res >> res_value;
        }
    }
    while (!res.eof())
    {
        temp_2 << res_value << ' ';
        res >> res_value;
    }

    while (!temp_1.eof())
    {
        temp_2 << temp_1_value << ' ';
        temp_1 >> temp_1_value;
    }

    temp_1.close();
    temp_2.close();
    res.close();

    res.open("res_file.txt", std::ofstream::out | std::ofstream::trunc);
    {
        if (res.is_open())
        {
            res.close();
        }
    }
    if (!filesystem::copy_file("temp_2_file.txt", "res_file.txt", filesystem::copy_option::overwrite_existing))
    {
        return;
    }
}

int raed_portition_array(fstream& fs, int*& arr)
{
    arr = new int(RANGE_SIZE);
    int* temp_arr;
    int i;

    for (i = 0; i < RANGE_SIZE && !fs.eof();i++)
    {
        fs >> arr[i];
    }
    if (i == 1)
    {
        delete[] arr;
        return 0;
    }
    if (i != RANGE_SIZE)
    {
        temp_arr = new int[i];
        for (size_t j = 0; j < i; j++)
        {
            temp_arr[j] = arr[j];
        }
        delete[] arr;
        arr = temp_arr;
        return i - 1;
    }
    return RANGE_SIZE;
}

void Sorted(const string& file_name)
{
    fstream fs;
    fs.open(file_name, fstream::in);

    if (fs.is_open())
    {
        while (!fs.eof())
        {
            int* portition_1;
            int* portition_2;
            int size_1 = raed_portition_array(fs, portition_1);
            int size_2 = raed_portition_array(fs, portition_2);

            if (size_1 == 0 || size_2 == 0)
            {
                return;
            }
            cout << "Size №1: " << size_1 << endl << "Size №2: " << size_2 << endl << endl;

            quickSort(portition_1, 0, size_1 - 1);
            quickSort(portition_2, 0, size_2 - 1);
            merge_to_file(portition_1, portition_2, size_1, size_2);
            merge_file();
        }
        fs.close();
    }
}

void Read_Array(const string& file_name, int*& arr, const int& n)
{
    fstream fs;
    fs.open(file_name, fstream::in);

        if (fs.is_open())
        {
            arr = new int[n];
            for (int i = 0; i < n; i++)
            {
                fs >> arr[i];
            }
            fs.close();
        }
}

void write_random_array(const string& file_name)
{
    fstream fs;

    srand(time(nullptr));
    int left_wall = -100;
    int range = 50000;

    fs.open(file_name, fstream::out | ofstream::trunc);
    if (fs.is_open())
    {
        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            fs << (left_wall + rand() % range) << ' ';

        }
        fs.close();
    }
}

int main(int argc, char const* argv[])
{
    setlocale(LC_ALL, "ru");
    string file_name = "array_data.txt";

    write_random_array(file_name);
    cout << endl << "Массив сгенерирован" << endl;
    fstream res;
    res.open("res_file.txt", fstream::out | ofstream::trunc);
    res.close();

    auto start_process = chrono::high_resolution_clock::now();
    Sorted(file_name);
    auto stop_process = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = stop_process - start_process;
    cout << endl << "Затраченое время процесса: " << elapsed.count() << " секунд" << endl;

    return 0;
}
