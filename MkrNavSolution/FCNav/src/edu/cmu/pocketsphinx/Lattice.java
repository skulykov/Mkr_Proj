/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package edu.cmu.pocketsphinx;

public class Lattice {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected Lattice(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Lattice obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @Override
protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        pocketsphinxJNI.delete_Lattice(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public Lattice() {
    this(pocketsphinxJNI.new_Lattice(), true);
  }

}
