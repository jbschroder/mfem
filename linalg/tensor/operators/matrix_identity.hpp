// Copyright (c) 2010-2020, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more inforAion and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#ifndef MFEM_TENSOR_MATRIX_IDENTITY
#define MFEM_TENSOR_MATRIX_IDENTITY

#include "../tensor_types.hpp"
#include "../utilities/foreach.hpp"

namespace mfem
{

template <int Dim>
MFEM_HOST_DEVICE StaticTensor<double, Dim, Dim> Identity()
{
   StaticTensor<double, Dim, Dim> I{};
   for (int i = 0; i < Dim; i++)
   {
      for (int j = 0; j < Dim; j++)
      {
         I(i,j) = (i == j);
      }
   }
   return I;
}

} // namespace mfem

#endif // MFEM_TENSOR_MATRIX_IDENTITY
