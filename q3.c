#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

typedef struct merge_data{
    int *arr;
    int left;
    int right;
} merge_data;

void insertionSort(int arr[], int n){
    int i, j, k;
    for (i = 1; i < n; i++){
        k = arr[i];

        for(j = i-1 ; j>=0 && arr[j] > k; j--) arr[j+1] = arr[j];

        arr[j+1] = k;
    }
}

void merge(int arr[],int in,int mid,int end){

	int s1=mid-in+1;
	int s2=end-mid;

	int a1[s1], a2[s2];
	int i,j=0,k=0;
	for(i=0;i<s1;i++)
		a1[i]=arr[in+i];

	for(i=0;i<s2;i++)
		a2[i]=arr[mid+1+i];
	i=in;
	while(j<s1 && k<s2){
		if(a1[j]<a2[k]){
			arr[i]=a1[j];
			j++;
		}
		else{
			arr[i]=a2[k];
			k++;
		}
		i++;
	}

	if(j==s1){
		for(;k<s2;k++)
		{
			arr[i]=a2[k];
			i++;
		}
	}

	if(k==s2){
		for(;j<s1;j++){
			arr[i]=a1[j];
			i++;
		}
	}

	return;
}

void mergeSort1(int arr[] , int in , int end){

	if(in>=end)
		return;

	int mid=in+(end-in)/2;
	mergeSort1(arr,in,mid);
	mergeSort1(arr,mid+1,end);

	merge(arr,in,mid,end);

	return;
}

void mergeSort2(int arr[] , int in , int end){


    if(end-in < 5){
        insertionSort(arr+in, end-in+1);
        return;
    }

	int mid=in+(end-in)/2;

    pid_t lcpid,rcpid;

    lcpid = fork();
    if (lcpid<0)    {
        perror("Left child not created\n");
        exit(-1);
    }
    else if (lcpid==0){
        mergeSort2(arr,in,mid);
        exit(0);
    }

    rcpid = fork();
    if (rcpid<0){
        perror("Right child not created\n");
        exit(-1);
    }
    else if(rcpid==0){
        mergeSort2(arr,mid+1,end);
        exit(0);
    }

    int status;

    waitpid(lcpid, &status, 0);
    waitpid(rcpid, &status, 0);

	merge(arr,in,mid,end);

	return;
}

void *mergeSort3(void* val){
    merge_data* mval = (merge_data*) val;
    int in = mval->left;
    int end = mval->right;

    if(end-in < 5){
        insertionSort(mval->arr+in, end-in+1);
        pthread_exit(NULL);
        return NULL;
    }

    int mid=in+(end-in)/2;

    merge_data lefth;
    lefth.left = in;
    lefth.right = mid;
    lefth.arr = mval->arr;

    pthread_t thleft;
    pthread_create(&thleft, NULL, mergeSort3, &lefth);

    merge_data righth;
    righth.left = mid + 1;
    righth.right = end;
    righth.arr = mval->arr;

    pthread_t thright;
    pthread_create(&thright, NULL, mergeSort3, &righth);

    pthread_join(thleft, NULL);
    pthread_join(thright, NULL);
    merge(mval->arr, in, mid, end);
}

int main(){

    int shared_mem_id;
    key_t key = IPC_PRIVATE;
    int *shared_array;

	int n , i , arr[10005], brr[10005];

	scanf("%d",&n);

    // Memory to allocate
    size_t SHM_SIZE = sizeof(int)*n;

    // Create the shared memory.
    if ((shared_mem_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        _exit(1);
    }

    // Attach the shared memory to our data space.
    if ((shared_array = shmat(shared_mem_id, NULL, 0)) == (int *) -1)
    {
        perror("shmat");
        _exit(1);
    }

	for(i=0;i<n;i++){
		scanf("%d",&arr[i]);
        shared_array[i] = arr[i];
        brr[i] = arr[i];
    }

    merge_data mval;
    mval.arr = brr;
    mval.left = 0;
    mval.right = n-1;

    // Calculate the time taken by mergeSort1()
    clock_t t;
    t = clock();
    mergeSort1(arr,0,n-1);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

    printf("mergeSort1() took %f seconds to execute \n", time_taken);

    // Calculate the time taken by mergeSort2()
    clock_t t2;
    t2 = clock();
    mergeSort2(shared_array,0,n-1);
    t2 = clock() - t2;
    double time_taken2 = ((double)t2)/CLOCKS_PER_SEC; // in seconds

    printf("mergeSort2() took %f seconds to execute \n", time_taken2);


    pthread_t thread;
    // Calculate the time taken by mergeSort3()
    clock_t t3;
    t3 = clock();
    int err = pthread_create(&thread, NULL, mergeSort3, &mval);
    if (err) {
        printf("Failed to create main thread");
    }
    // Wait for thread, i.e. the full merge sort algo.
    pthread_join(thread, NULL);
    t3 = clock() - t3;
    double time_taken3 = ((double)t2)/CLOCKS_PER_SEC; // in seconds

    printf("mergeSort3() took %f seconds to execute \n", time_taken3);

    FILE *out_file1 = fopen("merge1.txt", "w");
    for(i=0;i<n;i++) fprintf(out_file1, "%d\n", arr[i]);
    FILE *out_file2 = fopen("merge2.txt", "w");
	for(i=0;i<n;i++) fprintf(out_file2, "%d\n",shared_array[i]);
    FILE *out_file3 = fopen("merge3.txt", "w");
    for(i=0;i<n;i++) fprintf(out_file3, "%d\n",mval.arr[i]);
	return 0;
}
