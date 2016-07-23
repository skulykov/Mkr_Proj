/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package edu.cmu.pocketsphinx;

public class Hypothesis {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected Hypothesis(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Hypothesis obj) {
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
        pocketsphinxJNI.delete_Hypothesis(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setHypstr(String value) {
    pocketsphinxJNI.Hypothesis_hypstr_set(swigCPtr, this, value);
  }

  public String getHypstr() {
    return pocketsphinxJNI.Hypothesis_hypstr_get(swigCPtr, this);
  }

  public void setUttid(String value) {
    pocketsphinxJNI.Hypothesis_uttid_set(swigCPtr, this, value);
  }

  public String getUttid() {
    return pocketsphinxJNI.Hypothesis_uttid_get(swigCPtr, this);
  }

  public void setBest_score(int value) {
    pocketsphinxJNI.Hypothesis_best_score_set(swigCPtr, this, value);
  }

  public int getBest_score() {
    return pocketsphinxJNI.Hypothesis_best_score_get(swigCPtr, this);
  }

  public Hypothesis(String hypstr, String uttid, int best_score) {
    this(pocketsphinxJNI.new_Hypothesis(hypstr, uttid, best_score), true);
  }

}