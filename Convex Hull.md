1. Jarvis' March      
（ Gift Wrapping Algorithm ）
從一個凸包上的頂點開始，順著外圍繞一圈，順時針或逆時針都可以。  
每當尋找下一個要被包覆的點，則窮舉平面上所有點，找出最外圍的一點來包覆（可利用叉積運算來判斷）。   
時間複雜度為 O(N*M) ， N 為所有點的數目， M 為凸包的頂點數目。   

2.Graham's Scan   
  凸包上的頂點很有順序的沿著外圍繞行一圈，這個順序是時針順序。嘗試先將所有點按照時針順序排好，再來做繞一圈的動作，繞行途中順便掃除凸包內的點，如此就不必以窮舉所有點的方式來尋找最外圍的點。  
  
3.Andrew's Monotone Chain    
4.Incremental Method   
5.Divide and Conquer    
   一開始將所有點以X座標位置排序。  
   Divide：將所有點分成左半部和右半部。  
   Conquer：左半部和右半部分別求凸包。  
   Combine：將兩個凸包合併成一個凸包。  
   
   一開始可以不必排序，只要把所有點分成兩等份即可。兩個凸包合併成一個凸包時，兩個凸包可能會重疊，仍然可以用 O(N)時間解決，不過演算法較複雜，此處省略之。  
   
6.Quick Hull Algorithm   
   原理是 Trial and Error 與 Divide and Conquer 。
   
   
7.Chan's Algorithm   
8.Melkman's Algorithm   
9.Convex Layers  
