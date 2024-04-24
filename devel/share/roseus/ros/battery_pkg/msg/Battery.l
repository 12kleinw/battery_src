;; Auto-generated. Do not edit!


(when (boundp 'battery_pkg::Battery)
  (if (not (find-package "BATTERY_PKG"))
    (make-package "BATTERY_PKG"))
  (shadow 'Battery (find-package "BATTERY_PKG")))
(unless (find-package "BATTERY_PKG::BATTERY")
  (make-package "BATTERY_PKG::BATTERY"))

(in-package "ROS")
;;//! \htmlinclude Battery.msg.html


(defclass battery_pkg::Battery
  :super ros::object
  :slots (_electricity _status ))

(defmethod battery_pkg::Battery
  (:init
   (&key
    ((:electricity __electricity) 0.0)
    ((:status __status) "")
    )
   (send-super :init)
   (setq _electricity (float __electricity))
   (setq _status (string __status))
   self)
  (:electricity
   (&optional __electricity)
   (if __electricity (setq _electricity __electricity)) _electricity)
  (:status
   (&optional __status)
   (if __status (setq _status __status)) _status)
  (:serialization-length
   ()
   (+
    ;; float32 _electricity
    4
    ;; string _status
    4 (length _status)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _electricity
       (sys::poke _electricity (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; string _status
       (write-long (length _status) s) (princ _status s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _electricity
     (setq _electricity (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; string _status
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _status (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;;
   self)
  )

(setf (get battery_pkg::Battery :md5sum-) "22b8bb2cbf4d2d5daca78fc9844d7901")
(setf (get battery_pkg::Battery :datatype-) "battery_pkg/Battery")
(setf (get battery_pkg::Battery :definition-)
      "float32 electricity
string status
")



(provide :battery_pkg/Battery "22b8bb2cbf4d2d5daca78fc9844d7901")


