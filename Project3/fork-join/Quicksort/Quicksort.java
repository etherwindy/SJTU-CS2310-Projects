import java.util.concurrent.*;

public class Quicksort
{
  static class Sort extends RecursiveAction
  {
    private int begin;
    private int end;
    private int[] array;

    public Sort(int begin, int end, int[] array) {
      this.begin = begin;
      this.end = end;
      this.array = array;
    }

    protected void compute() {
      if(begin >= end)
      return;

      int mid_value = array[end];
      int left = begin;
      int right = end;

      while(left < right) {
        while(left < right && array[left] < mid_value)
        left++;
        while(left < right && array[right] >= mid_value)
        right--;

        int temp = array[right];
        array[right] = array[left];
        array[left] = temp;
      }

      if(array[left] >= array[end]) {
        int temp = array[left];
        array[left] = array[end];
        array[end] = temp;
      }
      else left++;

      Sort leftSort = new Sort(begin, left - 1, array);
      Sort rightSort = new Sort(left + 1, end, array);

      leftSort.fork();
      rightSort.fork();

      leftSort.join();
      rightSort.join();

      return;
    }
  }

  public static void main(String[] args) {
    int[] array = new int[]{1,3,7,6,2,4,5,8,9,0};
    int len = array.length;
    Sort Task = new Sort(0, len-1, array);
    Task.fork();
    Task.join();
    for(int i = 0; i < len; ++i) {
      System.out.printf("%d ", array[i]);
    }
    System.out.print("\n");
  }
}
