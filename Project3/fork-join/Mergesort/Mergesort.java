import java.util.concurrent.*;
import java.util.Arrays;

public class Mergesort
{
  static class Sort extends RecursiveTask<int[]>
{
    private int[] array;

    public Sort(int[] array) {
      this.array = array;
    }

    protected int[] compute() {
      if(array.length <= 2) {
        if(array.length == 2 && array[0] > array[1]) {
          int temp = array[1];
          array[1] = array[0];
          array[0] = temp;
        }
        return array;
      }

      int len = array.length;
      int mid = len >> 1;

      Sort leftTask = new Sort(Arrays.copyOf(array, mid));
      Sort rightTask = new Sort(Arrays.copyOfRange(array, mid, len));

      leftTask.fork();
      rightTask.fork();

      int[] result1 = leftTask.join();
      int[] result2 = rightTask.join();

      array = merge(result1, result2);
      return array;
    }

    private static int[] merge(int arr1[], int arr2[]) {
      int[] destArray = new int[arr1.length + arr2.length];
      int index1 = 0;
      int index2 = 0;
      int destIndex = 0;

      while(index1 < arr1.length && index2 < arr2.length) {
        if(arr1[index1] < arr2[index2])
        destArray[destIndex++] = arr1[index1++];
        else
        destArray[destIndex++] = arr2[index2++];
      }

      while(index1 < arr1.length) {
        destArray[destIndex++] = arr1[index1++];
      }

      while(index2 < arr2.length) {
        destArray[destIndex++] = arr2[index2++];
      }

      return destArray;
    }
  }

  public static void main(String[] args) {
    int[] array = new int[]{1,3,7,6,2,4,5,8,9,0};
    Sort Task = new Sort(array);
    Task.fork();
    int[] result = Task.join();
    for(int i = 0; i < result.length; ++i) {
      System.out.printf("%d ", result[i]);
    }
    System.out.print("\n");
  }
}
