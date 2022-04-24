
// Copyright 2022 YANG, YUN-TSE (yangacer).
// Distributed under the Boost Software License, Version 1.0.

#ifndef ENCAPSULATION_H_
#define ENCAPSULATION_H_

#define call(Ref, Mfn, ...) ((*(Ref).fnptr_->Mfn)((Ref).inst_, ##__VA_ARGS__))

#define is_valid_ref(Ref) ((Ref).inst_ != 0 && (Ref).fnptr_ != 0)

#endif
