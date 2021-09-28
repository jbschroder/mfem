// Copyright (c) 2010-2020, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#ifndef MFEM_TENSOR_GRAD
#define MFEM_TENSOR_GRAD

#include "tensor.hpp"
#include "../../general/backends.hpp"
#include "../dtensor.hpp"
#include "basis.hpp"
#include "contraction.hpp"
#include "concatenate.hpp"

namespace mfem
{

// Non-tensor
// template <int Dim, int D, int Q, typename Dofs>
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_non_tensor_basis<Basis>,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Grad<Basis> &basis, const Dofs &u_e)
{
   constexpr int basis_size = get_basis_capacity<Grad<Basis>>;
   MFEM_SHARED double s_G[basis_size];
   auto G = basis.GetG(s_G);

   constexpr int Q = get_basis_quads<Basis>;
   ResultTensor<Basis,Q> u(u_e); // TODO: Add a diff dim of 1?
   return G * u;
}

// 1D Tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 1,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Grad<Basis> &basis, const Dofs &u_e)
{
   constexpr int basis_size = get_basis_capacity<Grad<Basis>>;
   MFEM_SHARED double s_G[basis_size];
   auto G = basis.GetG(s_G);

   constexpr int Q = get_basis_quads<Basis>;
   ResultTensor<Basis,Q> u(u_e);
   return ContractX(G,u);
}

// 2D Tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 2,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Grad<Basis> &basis, const Dofs &u_e)
{
   constexpr int basis_size = get_basis_capacity<Grad<Basis>>;
   MFEM_SHARED double s_B[basis_size];
   auto B = basis.GetB(s_B);
   MFEM_SHARED double s_G[basis_size];
   auto G = basis.GetG(s_G);

   constexpr int Q = get_basis_quads<Basis>;
   ResultTensor<Basis,Q,Q> u(u_e);
   auto Bu = ContractX(B,u);
   auto Gu = ContractX(G,u);
   auto GBu = ContractY(G,Bu);
   auto BGu = ContractY(B,Gu);

   const int Q_r = basis.GetQuads();
   ResultTensor<Basis,Q,Q,2> Grad_u(Q_r,Q_r,2);
   Grad_u.template Get<2>(0) = BGu;
   Grad_u.template Get<2>(1) = GBu;
   return Grad_u;
}

// 3D Tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 3 &&
             false,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Grad<Basis> &basis, const Dofs &u_e)
{
   constexpr int basis_size = get_basis_capacity<Grad<Basis>>;
   MFEM_SHARED double s_B[basis_size];
   auto B = basis.GetB(s_B);
   MFEM_SHARED double s_G[basis_size];
   auto G = basis.GetG(s_G);

   constexpr int Q = get_basis_quads<Basis>;
   ResultTensor<Basis,Q,Q,Q> u(u_e);
   auto Bu = ContractX(B,u);
   auto Gu = ContractX(G,u);
   auto BBu = ContractY(B,Bu);
   auto BGu = ContractY(B,Gu);
   auto GBu = ContractY(G,Bu);
   auto BBGu = ContractZ(B,BGu);
   auto BGBu = ContractZ(B,GBu);
   auto GBBu = ContractZ(G,BBu);

   const int Q_r = basis.GetQuads();
   ResultTensor<Basis,Q,Q,Q,3> Grad_u(Q_r,Q_r,Q_r,3);
   Grad_u.template Get<2>(0) = BBGu;
   Grad_u.template Get<2>(1) = BGBu;
   Grad_u.template Get<2>(2) = GBBu;
   return Grad_u;
}

// Non-tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_non_tensor_basis<Basis>,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Trans<Grad<Basis>> &basis, const Dofs &u)
{
   constexpr int basis_size = get_basis_capacity<Trans<Grad<Basis>>>;
   MFEM_SHARED double s_G[basis_size];
   auto Gt = basis.GetGt(s_G);

   return Gt * u;
}

// 1D Tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 1,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Trans<Grad<Basis>> &basis, const Dofs &u)
{
   constexpr int basis_size = get_basis_capacity<Trans<Grad<Basis>>>;
   MFEM_SHARED double s_G[basis_size];
   auto Gt = basis.GetGt(s_G);

   return ContractX(Gt,u);
}

// 2D Tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 2,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Trans<Grad<Basis>> &basis, const Dofs &u)
{
   constexpr int Rank = get_tensor_rank<Dofs>;
   constexpr int Comp = Rank-1;
   constexpr int basis_size = get_basis_capacity<Trans<Grad<Basis>>>;
   MFEM_SHARED double s_B[basis_size];
   auto Bt = basis.GetBt(s_B);
   MFEM_SHARED double s_G[basis_size];
   auto Gt = basis.GetGt(s_G);

   auto ux = u.template Get<Comp>(0);
   auto Gux = ContractX(Gt,ux);
   auto v = ContractY(Bt,Gux);
   auto uy = u.template Get<Comp>(1);
   auto Buy = ContractX(Bt,uy);
   v += ContractY(Gt,Buy);
   return v;
}

// 3D Tensor
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 3 &&
             false,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Trans<Grad<Basis>> &basis, const Dofs &u)
{
   constexpr int Rank = get_tensor_rank<Dofs>;
   constexpr int Comp = Rank-1;
   constexpr int basis_size = get_basis_capacity<Trans<Grad<Basis>>>;
   MFEM_SHARED double s_B[basis_size];
   auto Bt = basis.GetBt(s_B);
   MFEM_SHARED double s_G[basis_size];
   auto Gt = basis.GetGt(s_G);

   auto ux = u.template Get<Comp>(0);
   auto Gux = ContractX(Gt,ux);
   auto BGux = ContractY(Bt,Gux);
   auto v = ContractZ(Bt,BGux);
   auto uy = u.template Get<Comp>(1);
   auto Buy = ContractX(Bt,uy);
   auto GBuy = ContractY(Gt,Buy);
   v += ContractZ(Bt,GBuy);
   auto uz = u.template Get<Comp>(2);
   auto Buz = ContractX(Bt,uz);
   auto BBuz = ContractY(Bt,Buz);
   v += ContractZ(Gt,BBuz);
   return v;
}

// 3D threaded version where each thread computes one value.
template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 3 &&
             true,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Grad<Basis> &basis, const Dofs &u)
{
   constexpr int basis_size = get_basis_capacity<Basis>;
   MFEM_SHARED double s_B[basis_size];
   MFEM_SHARED double s_G[basis_size];
   const auto B = basis.GetB(s_B);
   const auto G = basis.GetG(s_G);
   constexpr int D1D = get_basis_dofs<Basis>;
   constexpr int Q1D = get_basis_quads<Basis>;
   double Bqx[D1D], Bqy[D1D], Bqz[D1D];
   double Gqx[D1D], Gqy[D1D], Gqz[D1D];
   Static3dThreadDTensor<1,Q1D,Q1D,Q1D,3> Gu;
   MFEM_FOREACH_THREAD(qx,x,Q1D)
   {
      MFEM_FOREACH_THREAD(qy,y,Q1D)
      {
         MFEM_FOREACH_THREAD(qz,z,Q1D)
         {
            MFEM_UNROLL(D1D)
            for (int d = 0; d < D1D; d++)
            {
               Bqx[d] = B(qx,d);
               Bqy[d] = B(qy,d);
               Bqz[d] = B(qz,d);
               Gqx[d] = G(qx,d);
               Gqy[d] = G(qy,d);
               Gqz[d] = G(qz,d);
            }
            double du_dx = 0.0;
            double du_dy = 0.0;
            double du_dz = 0.0;
            MFEM_UNROLL(D1D)
            for (int dz = 0; dz < D1D; dz++)
            {
               MFEM_UNROLL(D1D)
               for (int dy = 0; dy < D1D; dy++)
               {
                  MFEM_UNROLL(D1D)
                  for (int dx = 0; dx < D1D; dx++)
                  {
                     const double val = u(dx,dy,dz);
                     du_dx += Gqx[dx] * Bqy[dy] * Bqz[dz] * val;
                     du_dy += Bqx[dx] * Gqy[dy] * Bqz[dz] * val;
                     du_dz += Bqx[dx] * Bqy[dy] * Gqz[dz] * val;
                  }
               }
            }
            Gu(qx,qy,qz,0) = du_dx;
            Gu(qx,qy,qz,1) = du_dy;
            Gu(qx,qy,qz,2) = du_dz;
         }
      }
   }
   return Gu;
}

template <typename Basis,
          typename Dofs,
          std::enable_if_t<
             is_tensor_basis<Basis> &&
             get_basis_dim<Basis> == 3 &&
             true,
             bool> = true >
MFEM_HOST_DEVICE inline
auto operator*(const Trans<Grad<Basis>> &basis, const Dofs &u)
{
   constexpr int Dim = 3;
   constexpr int basis_size = get_basis_capacity<Basis>;
   MFEM_SHARED double s_B[basis_size];
   MFEM_SHARED double s_G[basis_size];
   auto Bt = basis.GetBt(s_B);
   auto Gt = basis.GetGt(s_G);
   constexpr int D1D = get_basis_dofs<Basis>;
   constexpr int Q1D = get_basis_quads<Basis>;
   double Bdx[Q1D], Bdy[Q1D], Bdz[Q1D];
   double Gdx[Q1D], Gdy[Q1D], Gdz[Q1D];
   Static3dThreadDTensor<1,Q1D,Q1D,Q1D> Gtu;
   // Load u into shared memory
   MFEM_SHARED StaticDTensor<Q1D,Q1D,Q1D,Dim> s_u;
   MFEM_FOREACH_THREAD(qx,x,Q1D)
   {
      MFEM_FOREACH_THREAD(qy,y,Q1D)
      {
         MFEM_FOREACH_THREAD(qz,z,Q1D)
         {
            for (int d = 0; d < Dim; d++)
            {
               s_u(qx,qy,qz,d) = u(qx,qy,qz,d);
            }
         }
      }
   }
   MFEM_SYNC_THREAD;
   MFEM_FOREACH_THREAD(dx,x,D1D)
   {
      MFEM_FOREACH_THREAD(dy,y,D1D)
      {
         MFEM_FOREACH_THREAD(dz,z,D1D)
         {
            MFEM_UNROLL(Q1D)
            for (int q = 0; q < Q1D; q++)
            {
               Bdx[q] = Bt(dx,q);
               Bdy[q] = Bt(dy,q);
               Bdz[q] = Bt(dz,q);
               Gdx[q] = Gt(dx,q);
               Gdy[q] = Gt(dy,q);
               Gdz[q] = Gt(dz,q);
            }
            double res = 0.0;
            MFEM_UNROLL(Q1D)
            for (int qz = 0; qz < Q1D; qz++)
            {
               MFEM_UNROLL(Q1D)
               for (int qy = 0; qy < Q1D; qy++)
               {
                  MFEM_UNROLL(Q1D)
                  for (int qx = 0; qx < Q1D; qx++)
                  {
                     res += Gdx[qx] * Bdy[qy] * Bdz[qz] * s_u(qx,qy,qz,0);
                     res += Bdx[qx] * Gdy[qy] * Bdz[qz] * s_u(qx,qy,qz,1);
                     res += Bdx[qx] * Bdy[qy] * Gdz[qz] * s_u(qx,qy,qz,2);
                  }
               }
            }
            Gtu(dx,dy,dz) = res;
         }
      }
   }
   return Gtu;
}

} // namespace mfem

#endif // MFEM_TENSOR_GRAD
