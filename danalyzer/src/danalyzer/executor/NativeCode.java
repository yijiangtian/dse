/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package danalyzer.executor;

/**
 *
 * @author dan
 */
public class NativeCode {
  
  static {
    System.loadLibrary("danpatch");
  }
    
  /**
   * This native method performs the creation and initialization of an array of a given size and type.
   * 
   * @param ptr  - ptr to array allocation
   * @param size - the size of the array
   * @param type - data type of the array contents
   */
  public static native void newArrayNative(Value[] ptr, int size, int type);
}
