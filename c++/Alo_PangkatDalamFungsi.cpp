// alogaritma pangkat dalam fungsi

#include <iostream>

using namespace std;

int pangkat(int angka,int pangkat=2) //2 adalah nilai default
{
    int hasil=1;
    for(int i=1;i<=pangkat;i++){
        hasil= hasil*angka;
    }
    return hasil;
}

int main()
{
    cout<<pangkat(3)<<endl;
    cout<<pangkat(4,3)<<endl;
    cin.get();
    return 0;
}
